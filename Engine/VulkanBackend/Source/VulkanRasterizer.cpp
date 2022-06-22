// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRasterizer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanColorAttachment.hpp"
#include "VulkanBackend/VulkanDepthAttachment.hpp"

#include "Core/Utility/Hasher.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRasterizer::VulkanRasterizer(const std::shared_ptr<VulkanDevice>& pDevice, Camera& camera, uint32_t frameCount, std::vector<Core::AttachmentDescription>&& attachmentDescriptions, Core::Multisample multisample /*= Multisample::One*/, bool exclusiveBuffering /*= false*/)
			: Rasterizer(pDevice, camera, frameCount, std::move(attachmentDescriptions), multisample, exclusiveBuffering)
		{
			// Create the attachments.
			createAttachments();

			// Create the render pass.
			createRenderPass();

			// Create frame buffers.
			createFramebuffers();

			// Create the command buffers.
			m_pCommandBuffers = std::make_unique<VulkanCommandBuffers>(getDevice(), frameCount);

			// Make sure to set the object as valid.
			validate();
		}

		VulkanRasterizer::~VulkanRasterizer()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanRasterizer::terminate()
		{
			// Wait idle to finish everything we have prior to this.
			getDevice().as<VulkanDevice>()->waitIdle();

			// Destroy the command buffer.
			m_pCommandBuffers.reset();

			// Destroy the attachments.
			m_pAttachments.clear();

			// Destroy the frame buffers.
			destroyFramebuffers();

			// Destroy render pass.
			destroyRenderPass();

			invalidate();
		}

		void VulkanRasterizer::update()
		{
			// Begin the command buffer.
			m_pCommandBuffers->finishExecution();

			// Update everything ONLY if we have anything to update.
			if (needToUpdate())
			{
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

				// Unbind the rasterizer.
				m_pCommandBuffers->unbindRenderTarget();

				// End the command buffer.
				m_pCommandBuffers->end();

				// Reduce the variable.
				notifyUpdated();
			}

			// Submit and get the next command buffer.
			m_pCommandBuffers->submitGraphics();
			m_pCommandBuffers->next();
		}

		void VulkanRasterizer::updateExtent()
		{
			// Wait idle to finish everything we have prior to this.
			getDevice().as<VulkanDevice>()->waitIdle();

			// Destroy the previous attachments.
			m_pAttachments.clear();

			// Destroy the frame buffers and render pass.
			destroyFramebuffers();
			destroyRenderPass();

			// Create everything again.
			createAttachments();
			createRenderPass();
			createFramebuffers();

			// Reset the indexes.
			m_FrameIndex = 0;
			m_pCommandBuffers->resetIndex();
			toggleNeedToUpdate();
		}

		Flint::VulkanBackend::VulkanRenderTargetAttachment& VulkanRasterizer::getAttachment(uint32_t index)
		{
			return *m_pAttachments[m_ExclusiveBuffering * m_FrameIndex][index];
		}

		const Flint::VulkanBackend::VulkanRenderTargetAttachment& VulkanRasterizer::getAttachment(uint32_t index) const
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
					if (attachment.m_Type == Core::AttachmentType::Color)
					{
						if (attachment.m_Format == Core::PixelFormat::Undefined)
						{
							// Find the best color format and return it.
							pAttachment.emplace_back(
								std::make_unique<VulkanColorAttachment>(
									getDevice(),
									getWidth(),
									getHeight(),
									Utility::GetPixelFormat(
										Utility::FindSupportedFormat(
											getDevice(),
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
							pAttachment.emplace_back(std::make_unique<VulkanColorAttachment>(getDevice(), getWidth(), getHeight(), attachment.m_Format, m_Multisample));
					}
					else
					{
						if (attachment.m_Format == Core::PixelFormat::Undefined)
						{
							// Find the best depth format and return it.
							pAttachment.emplace_back(
								std::make_unique<VulkanDepthAttachment>(
									getDevice(),
									getWidth(),
									getHeight(),
									Utility::GetPixelFormat(
										Utility::FindSupportedFormat(
											getDevice(),
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
							pAttachment.emplace_back(std::make_unique<VulkanDepthAttachment>(getDevice(), getWidth(), getHeight(), attachment.m_Format, m_Multisample));
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
				if (pVulkanAttachment->getType() == Core::AttachmentType::Color)
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

			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreateRenderPass(getDevice().as<VulkanDevice>()->getLogicalDevice(), &renderPassCreateInfo, nullptr, &m_RenderPass), "Failed to create render pass!");
		}

		void VulkanRasterizer::destroyRenderPass()
		{
			getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyRenderPass(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_RenderPass, nullptr);
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

				FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreateFramebuffer(getDevice().as<VulkanDevice>()->getLogicalDevice(), &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]), "Failed to create the frame buffer!");
			}
		}

		void VulkanRasterizer::destroyFramebuffers()
		{
			for (const auto framebuffer : m_Framebuffers)
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyFramebuffer(getDevice().as<VulkanDevice>()->getLogicalDevice(), framebuffer, nullptr);
		}
	}
}