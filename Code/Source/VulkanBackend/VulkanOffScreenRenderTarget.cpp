// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanCommandBufferList.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanImage.hpp"

#include "GeometryStore.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			PixelFormat GetPixelFormat(VkFormat format)
			{
				switch (format)
				{
				case VK_FORMAT_UNDEFINED:
					return PixelFormat::UNDEFINED;

				case VK_FORMAT_R8_SRGB:
					return PixelFormat::R8_SRGB;

				case VK_FORMAT_R8G8_SRGB:
					return PixelFormat::R8G8_SRGB;

				case VK_FORMAT_R8G8B8_SRGB:
					return PixelFormat::R8G8B8_SRGB;

				case VK_FORMAT_R8G8B8A8_SRGB:
					return PixelFormat::R8G8B8A8_SRGB;

				case VK_FORMAT_B8G8R8_SRGB:
					return PixelFormat::B8G8R8_SRGB;

				case VK_FORMAT_B8G8R8A8_SRGB:
					return PixelFormat::B8G8R8A8_SRGB;

				case VK_FORMAT_D16_UNORM:
					return PixelFormat::D16_SINT;

				case VK_FORMAT_D32_SFLOAT:
					return PixelFormat::D32_SFLOAT;

				default:
					FLINT_THROW_BACKEND_ERROR("Unsupported format!");
				}

				return PixelFormat::UNDEFINED;
			}
		}

		VulkanOffScreenRenderTarget::VulkanOffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, OffScreenRenderTargetAttachment attachments, UI32 threadCount)
			: OffScreenRenderTarget(pDevice, extent, bufferCount, pDevice->CreatePrimaryCommandBufferList(bufferCount), attachments, threadCount), vRenderTarget(pDevice->StaticCast<VulkanDevice>())
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			pSecondaryCommandBuffer = std::make_unique<VulkanCommandBufferList>(pDevice, bufferCount, pCommandBufferList);

			std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments;
			if ((mAttachments & OffScreenRenderTargetAttachment::COLOR_BUFFER) == OffScreenRenderTargetAttachment::COLOR_BUFFER)
			{
				// TODO
				//pColorBuffer = std::make_unique<VulkanColorBuffer>(vDevice, extent, bufferCount, pSwapChain->GetFormat());
				//pAttachments.push_back(&pResults.back()->StaticCast<VulkanImage>());
			}

			if ((mAttachments & OffScreenRenderTargetAttachment::DEPTH_BUFFER) == OffScreenRenderTargetAttachment::DEPTH_BUFFER)
			{
				pResults.push_back(pDevice->CreateImage(ImageType::DIMENSIONS_2, ImageUsage::DEPTH, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), PixelFormat::D16_SINT, 1, 1, nullptr));
				//pResults.push_back(pDevice->CreateImage(ImageType::DIMENSIONS_2, ImageUsage::DEPTH, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), _Helpers::GetPixelFormat(Utilities::FindDepthFormat(vDevice.GetPhysicalDevice())), 1, 1, nullptr));
				pAttachments.push_back(&pResults.back()->StaticCast<VulkanImage>());
			}

			std::vector<VkSubpassDependency> vDependencies{ 2 };

			vDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[0].dstSubpass = 0;
			vDependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			vDependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			vDependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			vDependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			vDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			vDependencies[1].srcSubpass = 0;
			vDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			vDependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			vDependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			vDependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			vDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			vRenderTarget.CreateRenderPass(pAttachments, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
			vRenderTarget.CreateFrameBuffer(pAttachments, extent, bufferCount);
			vRenderTarget.CreateSyncObjects(bufferCount);

			vInheritInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

			// Setup default clear color values.
			pClearValues[1].color.float32[0] = CREATE_COLOR_256(32.0f);
			pClearValues[1].color.float32[1] = CREATE_COLOR_256(32.0f);
			pClearValues[1].color.float32[2] = CREATE_COLOR_256(32.0f);
			pClearValues[1].color.float32[3] = 1.0f;
			pClearValues[0].depthStencil.depth = 1.0f;
			pClearValues[0].depthStencil.stencil = 0;
		}

		void VulkanOffScreenRenderTarget::Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget)
		{
			if (!pThisRenderTarget)
				pThisRenderTarget = shared_from_this();

			if (pScreenBoundRenderTarget)
			{
				VulkanScreenBoundRenderTarget& vScreenBoundRenderTarget = pScreenBoundRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();
				mFrameIndex = vScreenBoundRenderTarget.GetFrameIndex();

				// Begin the command buffer.
				auto& vScreenBoundCommandBuffer = pScreenBoundRenderTarget->GetCommandBufferList()->StaticCast<VulkanCommandBufferList>();
				vScreenBoundCommandBuffer.BindRenderTargetSecondary(pThisRenderTarget);

				vInheritInfo.renderPass = vRenderTarget.vRenderPass;
				vInheritInfo.framebuffer = vRenderTarget.vFrameBuffers[mFrameIndex];	// TODO

				// Begin secondary command buffers.
				auto& vCommandBufferList = *pSecondaryCommandBuffer;
				vCommandBufferList.VulkanBeginSecondaryCommandBuffer(mFrameIndex, &vInheritInfo);

				// Bind the volatile draw instances.
				for (const auto store : mVolatileDrawInstanceOrder)
				{
					FLINT_SETUP_PROFILER();

					if (!store->GetVertexBuffer() || !store->GetIndexBuffer())
						continue;

					vCommandBufferList.BindVertexBuffer(store->GetVertexBuffer());
					vCommandBufferList.BindIndexBuffer(store->GetIndexBuffer(), store->GetIndexSize());

					auto& pipelines = mVolatileDrawInstanceMap.at(store);

					// Iterate through the pipelines.
					for (auto& pipeline : pipelines)
					{
						pipeline->PrepareResourcesToDraw();
						vCommandBufferList.BindGraphicsPipeline(pipeline);

						const auto drawData = pipeline->GetDrawData();
						const auto drawDataIndex = pipeline->GetCurrentDrawIndex();

						// Bind draw data.
						for (UI64 i = 0; i < drawDataIndex; i++)
						{
							if (drawData.find(i) == drawData.end())
								continue;

							const auto draw = drawData.at(i);
							vCommandBufferList.BindDrawResources(pipeline, draw.pResourceMap);
							vCommandBufferList.BindDynamicStates(pipeline, draw.pDynamicStates);
							vCommandBufferList.IssueDrawCall(draw.mVertexOffset, draw.mVertexCount, draw.mIndexOffset, draw.mIndexCount);
						}
					}
				}

				// Bind the draw instances.
				for (UI64 itr = 0; itr < mDrawInstanceOrder.size(); itr++)
				{
					const auto drawOrder = mDrawInstanceOrder[itr];
					const auto drawInstanceMap = mDrawInstanceMaps[itr];

					for (const auto store : drawOrder)
					{
						FLINT_SETUP_PROFILER();

						vCommandBufferList.BindVertexBuffer(store->GetVertexBuffer());
						vCommandBufferList.BindIndexBuffer(store->GetIndexBuffer(), store->GetIndexSize());

						auto& pipelines = drawInstanceMap.at(store);

						// Iterate through the pipelines.
						for (auto& pipeline : pipelines)
						{
							pipeline->PrepareResourcesToDraw();
							vCommandBufferList.BindGraphicsPipeline(pipeline);

							const auto drawData = pipeline->GetDrawData();

							// Bind draw data.
							for (const auto draw : drawData)
							{
								vCommandBufferList.BindDrawResources(pipeline, draw.second.pResourceMap);
								vCommandBufferList.BindDynamicStates(pipeline, draw.second.pDynamicStates);
								vCommandBufferList.IssueDrawCall(draw.second.mVertexOffset, draw.second.mVertexCount, draw.second.mIndexOffset, draw.second.mIndexCount);
							}
						}
					}
				}

				// End buffer recording and submit it to execute.
				vCommandBufferList.EndBufferRecording();

				vScreenBoundCommandBuffer.AddSecondaryCommandBuffer(vCommandBufferList.GetCurrentCommandBuffer());
				vScreenBoundCommandBuffer.ExecuteSecondaryCommands();
				vScreenBoundCommandBuffer.UnbindRenderTarget();
			}
			else
			{
				// TODO
				IncrementFrameIndex();
			}
		}

		void VulkanOffScreenRenderTarget::Terminate()
		{
			vRenderTarget.Terminate();

			pDevice->DestroyCommandBufferList(pCommandBufferList);
			pDevice->DestroyCommandBufferList(pVolatileCommandBufferList);
			pDevice->DestroyCommandBufferList(std::move(pSecondaryCommandBuffer));

			for (auto pResult : pResults)
				pDevice->DestroyImage(pResult);
		}

		void VulkanOffScreenRenderTarget::BindVolatileInstances()
		{
		}

		void VulkanOffScreenRenderTarget::SecondaryCommandsWorker(DrawInstanceMap& drawInstanceMap, std::list<std::shared_ptr<GeometryStore>>& drawOrder, BinarySemaphore& binarySemaphore, CountingSemaphore& countingSemaphore, std::atomic<bool>& shouldRun)
		{
		}

		FColor4D VulkanOffScreenRenderTarget::GetClearColor() const
		{
			return FColor4D(pClearValues[0].color.float32[0], pClearValues[0].color.float32[1], pClearValues[0].color.float32[2], pClearValues[0].color.float32[3]);
		}

		void VulkanOffScreenRenderTarget::SetClearColor(const FColor4D& newColor)
		{
			pClearValues[0].color.float32[0] = newColor.mRed;
			pClearValues[0].color.float32[1] = newColor.mGreen;
			pClearValues[0].color.float32[2] = newColor.mBlue;
			pClearValues[0].color.float32[3] = newColor.mAlpha;
		}

		VkFramebuffer VulkanOffScreenRenderTarget::GetFrameBuffer(UI32 index) const
		{
			return vRenderTarget.vFrameBuffers[index];
		}
	}
}

/*
Executed secondary command buffer using VkImage 0xb09e9c0000000039[] (subresource: aspectMask 0x2 array layer 0, mip level 0) which expects layout VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL--instead, image current layout is VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL.
*/