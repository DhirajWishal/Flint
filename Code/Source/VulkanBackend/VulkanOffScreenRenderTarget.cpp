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

		VulkanOffScreenRenderTarget::VulkanOffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::vector<OffScreenResultSpecification>& specifications, UI32 threadCount)
			: OffScreenRenderTarget(pDevice, extent, bufferCount, pDevice->CreatePrimaryCommandBufferList(bufferCount), specifications, threadCount), vRenderTarget(pDevice->StaticCast<VulkanDevice>())
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			pSecondaryCommandBuffer = std::make_unique<VulkanCommandBufferList>(pDevice, bufferCount, pCommandBufferList);
			vFrameClusters.resize(bufferCount);

			UI32 imagesPerCluster = 0;
			std::vector<std::vector<VkImageView>> vImageViewClusters;

			std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments;
			for (const auto specification : mResultSpecification)
			{
				if (specification.mAttachment == OffScreenRenderTargetAttachment::COLOR_BUFFER)
				{
					// TODO
					//pColorBuffer = std::make_unique<VulkanColorBuffer>(vDevice, extent, bufferCount, pSwapChain->GetFormat());
					//pAttachments.push_back(&pResults.back()->StaticCast<VulkanImage>());
					//
					//if (specification.mLayerMode == OffScreenResultLayerMode::PER_FRAME)
					//{
					//	std::vector<VkImageView> vImageViews;
					//
					//	if (specification.mLayerCount > 1)
					//		for (UI32 i = 0; i < specification.mLayerCount; i++)
					//			vImageViews.push_back(pDepthImage->StaticCast<VulkanImage>().CreateLayerBasedImageView(imagesPerCluster));
					//
					//	vImageViewsToDestroy.insert(vImageViewsToDestroy.end(), vImageViews.begin(), vImageViews.end());
					//}
				}
				else if (specification.mAttachment == OffScreenRenderTargetAttachment::DEPTH_BUFFER)
				{
					std::shared_ptr<Image> pDepthImage = nullptr;

					if (specification.mPixelFormat == PixelFormat::UNDEFINED || (specification.mPixelFormat != PixelFormat::D16_SINT && specification.mPixelFormat != PixelFormat::D32_SFLOAT))
						pDepthImage = pDevice->CreateImage(ImageType::DIMENSIONS_2, ImageUsage::DEPTH, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), _Helpers::GetPixelFormat(Utilities::FindDepthFormat(vDevice.GetPhysicalDevice())), 1, 1, nullptr);
					else
						pDepthImage = pDevice->CreateImage(ImageType::DIMENSIONS_2, ImageUsage::DEPTH, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), specification.mPixelFormat, specification.mLayerCount, 1, nullptr);

					pResults.push_back(pDepthImage);
					pAttachments.push_back(&pDepthImage->StaticCast<VulkanImage>());

					if (specification.mLayerMode == OffScreenResultLayerMode::PER_FRAME)
					{
						std::vector<VkImageView> vImageViews;

						if (specification.mLayerCount > 1)
							for (UI32 i = 0; i < specification.mLayerCount; i++)
								vImageViews.push_back(pDepthImage->StaticCast<VulkanImage>().CreateLayerBasedImageView(i));

						vImageViewClusters.push_back(std::move(vImageViews));
						imagesPerCluster = specification.mLayerCount;
					}
				}
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

			if (vImageViewClusters.empty())
			{
				vRenderTarget.CreateFrameBuffer(pAttachments, extent, bufferCount);

				for (UI64 index = 0; index < vRenderTarget.vFrameBuffers.size(); index++)
					vFrameClusters[index].push_back(vRenderTarget.vFrameBuffers[index]);

				vRenderTarget.vFrameBuffers.clear();
			}
			else
			{
				for (UI64 frameIndex = 0; frameIndex < bufferCount; frameIndex++)
				{
					std::vector<VkFramebuffer> vFrameBuffers = {};
					for (UI64 imageIndex = 0; imageIndex < imagesPerCluster; imageIndex++)
					{
						std::vector<VkImageView> vImageViews;
						for (const auto cluster : vImageViewClusters)
							vImageViews.push_back(cluster[imageIndex]);

						vFrameBuffers.push_back(vRenderTarget.CreateVulkanFrameBuffer(extent, std::move(vImageViews)));
					}

					vFrameClusters.push_back(vFrameBuffers);
				}

				for (auto& cluster : vImageViewClusters)
					vImageViewsToDestroy.insert(vImageViewsToDestroy.end(), cluster.begin(), cluster.end());
				vImageViewClusters.clear();
			}

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

				// Begin the command buffer.
				auto& vScreenBoundCommandBuffer = pScreenBoundRenderTarget->GetCommandBufferList()->StaticCast<VulkanCommandBufferList>();
				mFrameIndex = vScreenBoundRenderTarget.GetFrameIndex();

				// Iterate through frame buffers in a cluster.
				for (UI32 index = 0; index < vFrameClusters[mFrameIndex].size(); index++)
				{
					vInheritInfo.renderPass = vRenderTarget.vRenderPass;
					vInheritInfo.framebuffer = GetFrameBuffer(index);

					vScreenBoundCommandBuffer.VulkanBindRenderTargetSecondary(pThisRenderTarget, vInheritInfo.framebuffer);

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

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			vDevice.DestroyCommandBufferList(pCommandBufferList);
			vDevice.DestroyCommandBufferList(pVolatileCommandBufferList);
			vDevice.DestroyCommandBufferList(std::move(pSecondaryCommandBuffer));

			for (auto pResult : pResults)
				vDevice.DestroyImage(pResult);

			for (const auto vImageViewCluster : vFrameClusters)
				for (const auto vImageView : vImageViewCluster)
					vkDestroyFramebuffer(vDevice.GetLogicalDevice(), vImageView, nullptr);

			vFrameClusters.clear();

			for (const auto vImageView : vImageViewsToDestroy)
				vkDestroyImageView(vDevice.GetLogicalDevice(), vImageView, nullptr);

			vImageViewsToDestroy.clear();
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
			return vFrameClusters[mFrameIndex][index];
		}
	}
}

/*
vkCmdExecuteCommands() called w/ invalid secondary VkCommandBuffer 0x1fac70410b8[] which has a VkFramebuffer 0xf37618000000003d[] that is not the same as the primary command buffer's current active VkFramebuffer 0xc1547b000000003e[]. The Vulkan spec states: If vkCmdExecuteCommands is being called within a render pass instance, and any element of pCommandBuffers was recorded with VkCommandBufferInheritanceInfo::framebuffer not equal to VK_NULL_HANDLE, that VkFramebuffer must match the VkFramebuffer used in the current render pass instance (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkCmdExecuteCommands-pCommandBuffers-00099)
Calling vkBeginCommandBuffer() on active VkCommandBuffer 0x1fac70410b8[] before it has completed. You must check command buffer fence before this call. The Vulkan spec states: commandBuffer must not be in the recording or pending state (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkBeginCommandBuffer-commandBuffer-00049)

vkCmdExecuteCommands() called w/ invalid secondary VkCommandBuffer 0x1fac70410b8[] which has a VkFramebuffer 0xf37618000000003d[] that is not the same as the primary command buffer's current active VkFramebuffer 0xc1547b000000003e[]. The Vulkan spec states: If vkCmdExecuteCommands is being called within a render pass instance, and any element of pCommandBuffers was recorded with VkCommandBufferInheritanceInfo::framebuffer not equal to VK_NULL_HANDLE, that VkFramebuffer must match the VkFramebuffer used in the current render pass instance (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkCmdExecuteCommands-pCommandBuffers-00099)
Calling vkBeginCommandBuffer() on active VkCommandBuffer 0x1fac70410b8[] before it has completed. You must check command buffer fence before this call. The Vulkan spec states: commandBuffer must not be in the recording or pending state (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkBeginCommandBuffer-commandBuffer-00049)

vkCmdExecuteCommands() called w/ invalid secondary VkCommandBuffer 0x1fac70410b8[] which has a VkFramebuffer 0xf37618000000003d[] that is not the same as the primary command buffer's current active VkFramebuffer 0xd20a5a000000003f[]. The Vulkan spec states: If vkCmdExecuteCommands is being called within a render pass instance, and any element of pCommandBuffers was recorded with VkCommandBufferInheritanceInfo::framebuffer not equal to VK_NULL_HANDLE, that VkFramebuffer must match the VkFramebuffer used in the current render pass instance (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkCmdExecuteCommands-pCommandBuffers-00099)
Calling vkBeginCommandBuffer() on active VkCommandBuffer 0x1fac70410b8[] before it has completed. You must check command buffer fence before this call. The Vulkan spec states: commandBuffer must not be in the recording or pending state (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkBeginCommandBuffer-commandBuffer-00049)

vkCmdExecuteCommands() called w/ invalid secondary VkCommandBuffer 0x1fac70410b8[] which has a VkFramebuffer 0xf37618000000003d[] that is not the same as the primary command buffer's current active VkFramebuffer 0xd20a5a000000003f[]. The Vulkan spec states: If vkCmdExecuteCommands is being called within a render pass instance, and any element of pCommandBuffers was recorded with VkCommandBufferInheritanceInfo::framebuffer not equal to VK_NULL_HANDLE, that VkFramebuffer must match the VkFramebuffer used in the current render pass instance (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkCmdExecuteCommands-pCommandBuffers-00099)
Calling vkBeginCommandBuffer() on active VkCommandBuffer 0x1fac70410b8[] before it has completed. You must check command buffer fence before this call. The Vulkan spec states: commandBuffer must not be in the recording or pending state (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkBeginCommandBuffer-commandBuffer-00049)
*/