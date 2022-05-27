// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRasterizer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanColorAttachment.hpp"
#include "VulkanBackend/VulkanDepthAttachment.hpp"
#include "VulkanBackend/VulkanGeometryStore.hpp"

#include "Core/Utility/Hasher.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRasterizer::VulkanRasterizer(Engine& engine, uint32_t width, uint32_t height, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample /*= Multisample::One*/, bool exclusiveBuffering /*= false*/)
			: Rasterizer(engine, width, height, frameCount, std::move(attachmentDescriptions), multisample, exclusiveBuffering)
		{
			// Create the attachments.
			createAttachments();

			// Create the render pass.
			createRenderPass();

			// Create frame buffers.
			createFramebuffers();

			// Create the command buffers.
			m_pCommandBuffers = std::make_unique<VulkanCommandBuffers>(getEngineAs<VulkanEngine>(), frameCount);
		}

		VulkanRasterizer::~VulkanRasterizer()
		{
			// Wait idle to finish everything we have prior to this.
			getEngine().waitIdle();

			// Destroy the frame buffers.
			destroyFramebuffers();

			// Destroy render pass.
			destroyRenderPass();
		}

		void VulkanRasterizer::update()
		{
			// Begin the command buffer.
			m_pCommandBuffers->begin();

			// Bind the rasterizer.
			VkClearValue colorClearValue = {};
			colorClearValue.color.float32[0] = 1.0f;
			colorClearValue.color.float32[1] = 1.0f;
			colorClearValue.color.float32[2] = 1.0f;
			colorClearValue.color.float32[3] = 1.0f;

			VkClearValue depthClearValue = {};
			depthClearValue.depthStencil.depth = 1.0f;
			depthClearValue.depthStencil.stencil = 0;

			m_pCommandBuffers->bindRenderTarget(*this, { colorClearValue, depthClearValue });

			// Bind the index buffer.
			const auto& defaultGeometryStore = *getEngineAs<VulkanEngine>().getDefaultGeometryStore().as<VulkanGeometryStore>();
			m_pCommandBuffers->bindIndexBuffer(defaultGeometryStore);

			// Bind the resources.
			for (const auto& [hash, group] : m_Pipelines)
			{
				for (const auto& entry : group.m_DrawEntries)
				{
					const auto& meshses = entry.m_Geometry.getMeshes();
					for (uint32_t i = 0; i < entry.m_BindingTables.size(); ++i)
					{
						const auto& mesh = meshses[i];
						const auto meshHash = GenerateHash(mesh);
						const auto& bindingTable = entry.m_BindingTables[i];
						const auto& pipeline = *group.m_pPipelines[group.m_MeshPipelineHashes.at(meshHash)];
						const auto& descriptorSet = pipeline.getDescriptorSet(bindingTable);

						// Bind the pipeline.
						m_pCommandBuffers->bindGraphicsPipeline(pipeline);
						m_pCommandBuffers->bindDescriptor(pipeline, descriptorSet);

						// Draw the mesh
						m_pCommandBuffers->drawMesh(defaultGeometryStore, mesh);
					}
				}
			}

			// Unbind the rasterizer.
			m_pCommandBuffers->unbindRenderTarget();

			// End the command buffer.
			m_pCommandBuffers->end();

			// Submit and get the next command buffer.
			m_pCommandBuffers->submitGraphics();
			m_pCommandBuffers->next();
		}

		void VulkanRasterizer::resize(uint32_t width, uint32_t height)
		{
			// Return if we have the same width and height.
			if (getWidth() == width && getHeight() == height)
				return;

			// Wait idle to finish everything we have prior to this.
			getEngine().waitIdle();

			m_Width = width;
			m_Height = height;

			// Destroy the previous attachments.
			m_pAttachments.clear();

			// Destroy the frame buffers and render pass.
			destroyFramebuffers();
			destroyRenderPass();

			// Create everything again.
			createAttachments();
			createRenderPass();
			createFramebuffers();

			// Recreate the pipelines.
			for (auto& [hash, group] : m_Pipelines)
			{
				for (auto& pipeline : group.m_pPipelines)
					pipeline->recreate();
			}

			// Reset the indexes.
			m_FrameIndex = 0;
			m_pCommandBuffers->resetIndex();
		}

		void VulkanRasterizer::registerGeometry(const Geometry& geometry, RasterizingPipelineSpecification&& specification, std::function<ResourceBindingTable(const Mesh&, const Geometry&, const std::vector<ResourceBinding>&)>&& meshBinder)
		{
			auto& group = m_Pipelines[GenerateHash(specification)];
			auto& drawEntry = group.m_DrawEntries.emplace_back();
			drawEntry.m_Geometry = geometry;

			auto resourceBindings = specification.m_VertexShader.getBindings();
			resourceBindings.insert(resourceBindings.end(), specification.m_FragmentShader.getBindings().begin(), specification.m_FragmentShader.getBindings().end());

			for (const auto& mesh : geometry.getMeshes())
			{
				// Get the binding table.
				auto bindingTable = meshBinder(mesh, geometry, resourceBindings);

				std::vector<VkVertexInputBindingDescription> bindingDescriptions;
				std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

				// Validate the vertex inputs and generate the descriptions if possible.
				if (specification.m_VertexShader.hasVertexInputs())
				{
					auto& vertexBinding = bindingDescriptions.emplace_back();
					vertexBinding.binding = 0;
					vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
					vertexBinding.stride = mesh.getVertexStride();

					uint32_t offset = 0;
					const auto descriptor = mesh.getVertexDescriptor();
					auto descriptorIterator = descriptor.begin();
					for (const auto& attribute : specification.m_VertexShader.getVertexInputs())
					{
						// Iterate the iterator till we get to the right attribute.
						while (*descriptorIterator != attribute.m_DataType)
						{
							// Throw an error if we reached the end, because that means that we don't have what it needs to build the pipeline.
							if (descriptorIterator == descriptor.end())
								throw BackendError("One or more required attributes, or attributes with the required type was not found! Make sure that the mesh(s) contain the required attributes.");

							offset += DataTypeSize[EnumToInt(*descriptorIterator)];
							++descriptorIterator;
						}

						auto& vertexAttribute = attributeDescriptions.emplace_back();
						vertexAttribute.binding = vertexBinding.binding;
						vertexAttribute.location = attribute.m_Location;
						vertexAttribute.offset = offset;
						vertexAttribute.format = Utility::GetVkFormat(attribute.m_DataType);

						offset += DataTypeSize[EnumToInt(attribute.m_DataType)];
						++descriptorIterator;
					}
				}

				// Validate the instance inputs and generate the descriptions if possible.
				if (specification.m_VertexShader.hasInstanceInputs())
				{
					auto& vertexBinding = bindingDescriptions.emplace_back();
					vertexBinding.binding = 1;
					vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
					vertexBinding.stride = GetStride(mesh.getInstanceDescriptor());

					uint32_t offset = 0;
					const auto descriptor = mesh.getInstanceDescriptor();
					auto descriptorIterator = descriptor.begin();
					for (const auto& attribute : specification.m_VertexShader.getInstanceInputs())
					{
						// Iterate the iterator till we get to the right attribute.
						while (*descriptorIterator != attribute.m_DataType)
						{
							// Throw an error if we reached the end, because that means that we don't have what it needs to build the pipeline.
							if (descriptorIterator == descriptor.end())
								throw BackendError("One or more required attributes, or attributes with the required type was not found! Make sure that the mesh(s) contain the required attributes.");

							offset += DataTypeSize[EnumToInt(*descriptorIterator)];
							++descriptorIterator;
						}

						auto& vertexAttribute = attributeDescriptions.emplace_back();
						vertexAttribute.binding = vertexBinding.binding;
						vertexAttribute.location = attribute.m_Location;
						vertexAttribute.offset = offset;
						vertexAttribute.format = Utility::GetVkFormat(attribute.m_DataType);

						offset += DataTypeSize[EnumToInt(attribute.m_DataType)];
						++descriptorIterator;
					}
				}

				// If the pipeline is present, we don't need to create one. If not we need to create a new one.
				const auto meshHash = GenerateHash(mesh);
				if (!group.m_MeshPipelineHashes.contains(meshHash))
					group.m_MeshPipelineHashes.emplace(meshHash, group.m_pPipelines.emplace(std::make_unique<VulkanGraphicsPipeline>(getEngineAs<VulkanEngine>(), *this, specification, std::move(bindingDescriptions), std::move(attributeDescriptions))).first);

				// Register the binding table.
				group.m_pPipelines[group.m_MeshPipelineHashes[meshHash]]->registerTable(bindingTable);
				drawEntry.m_BindingTables.emplace_back(bindingTable);
			}
		}

		Flint::RenderTargetAttachment& VulkanRasterizer::getAttachment(uint32_t index)
		{
			return *m_pAttachments[m_ExclusiveBuffering * m_FrameIndex][index];
		}

		const Flint::RenderTargetAttachment& VulkanRasterizer::getAttachment(uint32_t index) const
		{
			return *m_pAttachments[m_ExclusiveBuffering * m_FrameIndex][index];
		}

		void VulkanRasterizer::createAttachments()
		{
			if (m_ExclusiveBuffering)
				m_pAttachments.resize(m_FrameCount);

			else
				m_pAttachments.resize(1);

			// Iterate over and create the attachments.
			for (auto& pAttachment : m_pAttachments)
			{
				for (const auto attachment : m_AttachmentDescriptions)
				{
					if (attachment.m_Type == AttachmentType::Color)
					{
						if (attachment.m_Format == PixelFormat::Undefined)
						{
							// Find the best color format and return it.
							pAttachment.emplace_back(
								std::make_unique<VulkanColorAttachment>(
									getEngine(),
									m_Width,
									m_Height,
									Utility::GetPixelFormat(
										Utility::FindSupportedFormat(
											getEngineAs<VulkanEngine>(),
											{ VK_FORMAT_R8G8B8A8_UNORM },
											VK_IMAGE_TILING_OPTIMAL,
											VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
											VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
											VK_FORMAT_FEATURE_TRANSFER_DST_BIT
										)
									),
									m_Multisample)
							);
						}
						else
							pAttachment.emplace_back(std::make_unique<VulkanColorAttachment>(getEngine(), m_Width, m_Height, attachment.m_Format, m_Multisample));
					}
					else
					{
						if (attachment.m_Format == PixelFormat::Undefined)
						{
							// Find the best depth format and return it.
							pAttachment.emplace_back(
								std::make_unique<VulkanDepthAttachment>(
									getEngine(),
									m_Width,
									m_Height,
									Utility::GetPixelFormat(
										Utility::FindSupportedFormat(
											getEngineAs<VulkanEngine>(),
											{ VK_FORMAT_D16_UNORM, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
											VK_IMAGE_TILING_OPTIMAL,
											VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
											VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
											VK_FORMAT_FEATURE_TRANSFER_DST_BIT
										)
									),
									m_Multisample)
							);
						}
						else
							pAttachment.emplace_back(std::make_unique<VulkanDepthAttachment>(getEngine(), m_Width, m_Height, attachment.m_Format, m_Multisample));
					}
				}
			}
		}

		void VulkanRasterizer::createRenderPass()
		{
			// Create attachment descriptions and references.
			std::vector<VkAttachmentReference> colorAttachments;
			std::vector<VkAttachmentReference> depthAttachments;
			std::vector<VkAttachmentDescription> attachmentDescriptions;

			VkAttachmentReference attachmentReference = {};

			// Note that here we just need to get them from one attachment, because it's the same for the rest.
			for (const auto& pAttachment : m_pAttachments[0])
			{
				// Get the attachment.
				const auto pVulkanAttachment = pAttachment.get();
				attachmentDescriptions.emplace_back(pVulkanAttachment->getAttachmentDescription());

				// Add it as a color attachment if its a color attachment. If not, as a depth attachment.
				if (pVulkanAttachment->getType() == AttachmentType::Color)
				{
					attachmentReference.layout = pVulkanAttachment->getLayout();
					colorAttachments.emplace_back(attachmentReference);
				}
				else
				{
					attachmentReference.layout = pVulkanAttachment->getLayout();
					depthAttachments.emplace_back(attachmentReference);
				}

				// Increment the attachment count.
				attachmentReference.attachment++;
			}

			// Create the subpass dependencies.
			std::array<VkSubpassDependency, 2> subpassDependencies;
			subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencies[0].dstSubpass = 0;
			subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			subpassDependencies[1].srcSubpass = 0;
			subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			// Create the subpass description.
			VkSubpassDescription subpassDescription = {};
			subpassDescription.flags = 0;
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.inputAttachmentCount = 0;
			subpassDescription.pInputAttachments = nullptr;
			subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
			subpassDescription.pColorAttachments = colorAttachments.data();
			subpassDescription.pResolveAttachments = nullptr;
			subpassDescription.pDepthStencilAttachment = depthAttachments.data();
			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = nullptr;

			// Create the render target.
			VkRenderPassCreateInfo renderPassCreateInfo = {};
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.pNext = nullptr;
			renderPassCreateInfo.flags = 0;
			renderPassCreateInfo.attachmentCount = attachmentReference.attachment;
			renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
			renderPassCreateInfo.subpassCount = 1;
			renderPassCreateInfo.pSubpasses = &subpassDescription;
			renderPassCreateInfo.dependencyCount = 2;
			renderPassCreateInfo.pDependencies = subpassDependencies.data();

			FLINT_VK_ASSERT(getEngineAs<VulkanEngine>().getDeviceTable().vkCreateRenderPass(getEngineAs<VulkanEngine>().getLogicalDevice(), &renderPassCreateInfo, nullptr, &m_RenderPass), "Failed to create render pass!");
		}

		void VulkanRasterizer::destroyRenderPass()
		{
			getEngineAs<VulkanEngine>().getDeviceTable().vkDestroyRenderPass(getEngineAs<VulkanEngine>().getLogicalDevice(), m_RenderPass, nullptr);
		}

		void VulkanRasterizer::createFramebuffers()
		{
			VkFramebufferCreateInfo frameBufferCreateInfo = {};
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.pNext = VK_NULL_HANDLE;
			frameBufferCreateInfo.flags = 0;
			frameBufferCreateInfo.renderPass = m_RenderPass;
			frameBufferCreateInfo.width = getWidth();
			frameBufferCreateInfo.height = getHeight();
			frameBufferCreateInfo.layers = 1;

			// Iterate and create the frame buffers.
			m_Framebuffers.resize(getFrameCount());
			for (uint8_t i = 0; i < getFrameCount(); i++)
			{
				// Get the image views.
				std::vector<VkImageView> imageViews;
				imageViews.reserve(m_pAttachments.size());
				for (const auto& pAttachment : m_pAttachments[m_ExclusiveBuffering * i])
					imageViews.emplace_back(pAttachment.get()->getImageView());

				frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
				frameBufferCreateInfo.pAttachments = imageViews.data();

				FLINT_VK_ASSERT(getEngineAs<VulkanEngine>().getDeviceTable().vkCreateFramebuffer(getEngineAs<VulkanEngine>().getLogicalDevice(), &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]), "Failed to create the frame buffer!");
			}
		}

		void VulkanRasterizer::destroyFramebuffers()
		{
			for (const auto framebuffer : m_Framebuffers)
				getEngineAs<VulkanEngine>().getDeviceTable().vkDestroyFramebuffer(getEngineAs<VulkanEngine>().getLogicalDevice(), framebuffer, nullptr);
		}
	}
}