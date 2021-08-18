// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/OffScreenRenderTargets/VulkanPointShadowMap.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "GeometryStore.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanPointShadowMap::VulkanPointShadowMap(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, UI32 threadCount)
			: VulkanOffScreenRenderTarget(pDevice, OffScreenRenderTargetType::POINT_SHADOW_MAP, extent, bufferCount, threadCount)
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			pSecondaryCommandBuffer = std::make_unique<VulkanCommandBufferList>(pDevice, bufferCount * 6, pCommandBufferList);
			pResults.push_back(pDevice->CreateImage(ImageType::CUBEMAP, ImageUsage::GRAPHICS, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), PixelFormat::R32_SFLOAT, 6, 1, nullptr));

			pColorImage = std::make_unique<VulkanImage>(pDevice, ImageType::DIMENSIONS_2, ImageUsage::COLOR, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), PixelFormat::R32_SFLOAT, 1, 1, nullptr);
			pDepthImage = std::make_unique<VulkanImage>(pDevice, ImageType::DIMENSIONS_2, ImageUsage::DEPTH, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), PixelFormat::D16_SINT, 1, 1, nullptr);
			
			vRenderTarget.CreateRenderPass({ pColorImage.get(), pDepthImage.get() }, VK_PIPELINE_BIND_POINT_GRAPHICS, {});
			vRenderTarget.CreateFrameBuffer({ pColorImage.get(), pDepthImage.get() }, mExtent, bufferCount);
			vRenderTarget.CreateSyncObjects(bufferCount);

			pClearValues[0].color.float32[0] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[1] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[2] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[3] = 1.0f;
			pClearValues[1].depthStencil.depth = 1.0f;
			pClearValues[1].depthStencil.stencil = 0;
		}

		void VulkanPointShadowMap::Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget)
		{
			if (!pThisRenderTarget)
				pThisRenderTarget = shared_from_this();

			if (pScreenBoundRenderTarget)
			{
				VulkanScreenBoundRenderTarget& vScreenBoundRenderTarget = pScreenBoundRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();
				VulkanImage& vResultImage = pResults.back()->StaticCast<VulkanImage>();

				// Begin the command buffer.
				auto& vScreenBoundCommandBuffer = pScreenBoundRenderTarget->GetCommandBufferList()->StaticCast<VulkanCommandBufferList>();
				auto& vCommandBufferList = *pSecondaryCommandBuffer;

				mFrameIndex = vScreenBoundRenderTarget.GetFrameIndex();

				for (UI8 face = 0; face < 6; face++)
				{
					// Iterate through frame buffers in a cluster.
					vInheritInfo.renderPass = vRenderTarget.vRenderPass;
					vInheritInfo.framebuffer = GetFrameBuffer(mFrameIndex);

					vScreenBoundCommandBuffer.VulkanBindRenderTargetSecondary(pThisRenderTarget, vInheritInfo.framebuffer);
					vCommandBufferList.VulkanBeginSecondaryCommandBuffer((mFrameIndex * 6) + face, &vInheritInfo);

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

							// Check if the draw data count is valid.
							if (drawData.size() % 6 != 0)
								FLINT_THROW_BACKEND_ERROR("Invalid draw count! When drawing objects to the point shadow map make sure that you have 6 instances of the same draw data so that the render target can draw each into a separate cube map face.");

							// Bind draw data.
							UI8 counter = 0;
							for (UI64 i = 0; i < drawDataIndex; i++)
							{
								if (drawData.find(i) == drawData.end())
									continue;

								if (counter != face)
								{
									counter = ++counter % 6;
									continue;
								}

								const auto draw = drawData.at(i);

								vCommandBufferList.BindDrawResources(pipeline, draw.pResourceMap);
								vCommandBufferList.BindDynamicStates(pipeline, draw.pDynamicStates);
								vCommandBufferList.IssueDrawCall(draw.mVertexOffset, draw.mVertexCount, draw.mIndexOffset, draw.mIndexCount);
								counter = ++counter % 6;
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
								const auto drawDataIndex = pipeline->GetCurrentDrawIndex();

								// Validate the draw data count.
								if (drawData.size() % 6 != 0)
									FLINT_THROW_BACKEND_ERROR("Invalid draw count! When drawing objects to the point shadow map make sure that you have 6 instances of the same draw data so that the render target can draw each into a separate cube map face.");

								// Bind draw data.
								UI8 counter = 0;
								for (UI64 i = 0; i < drawDataIndex; i++)
								{
									if (drawData.find(i) == drawData.end())
										continue;

									if (counter != face)
									{
										counter = ++counter % 6;
										continue;
									}

									const auto draw = drawData.at(i);

									vCommandBufferList.BindDrawResources(pipeline, draw.pResourceMap);
									vCommandBufferList.BindDynamicStates(pipeline, draw.pDynamicStates);
									vCommandBufferList.IssueDrawCall(draw.mVertexOffset, draw.mVertexCount, draw.mIndexOffset, draw.mIndexCount);
									counter = ++counter % 6;
								}
							}
						}
					}

					vCommandBufferList.EndBufferRecording();

					vScreenBoundCommandBuffer.AddSecondaryCommandBuffer(vCommandBufferList.GetCurrentCommandBuffer());
					vScreenBoundCommandBuffer.ExecuteSecondaryCommands();
					vScreenBoundCommandBuffer.UnbindRenderTarget();

					// Copy the image to the cube map.
					pColorImage->SetImageLayout(vScreenBoundCommandBuffer.GetCurrentCommandBuffer(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
					vResultImage.SetImageLayout(vScreenBoundCommandBuffer.GetCurrentCommandBuffer(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, face, 1);

					VkImageCopy vImageCopy = {};
					vImageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					vImageCopy.srcSubresource.baseArrayLayer = 0;
					vImageCopy.srcSubresource.mipLevel = 0;
					vImageCopy.srcSubresource.layerCount = 1;
					vImageCopy.srcOffset = { 0, 0, 0 };

					vImageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					vImageCopy.dstSubresource.baseArrayLayer = face;
					vImageCopy.dstSubresource.mipLevel = 0;
					vImageCopy.dstSubresource.layerCount = 1;
					vImageCopy.dstOffset = { 0, 0, 0 };

					vImageCopy.extent.width = mExtent.mWidth;
					vImageCopy.extent.height = mExtent.mHeight;
					vImageCopy.extent.depth = 1;

					vkCmdCopyImage(
						vScreenBoundCommandBuffer.GetCurrentCommandBuffer(),
						pColorImage->GetImage(),
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						vResultImage.GetImage(),
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1,
						&vImageCopy);

					pColorImage->SetImageLayout(vScreenBoundCommandBuffer.GetCurrentCommandBuffer(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
					vResultImage.SetImageLayout(vScreenBoundCommandBuffer.GetCurrentCommandBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, face, 1);
				}
			}
			else
			{
				// TODO
				IncrementFrameIndex();
			}
		}

		void VulkanPointShadowMap::Terminate()
		{
			vRenderTarget.Terminate();
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			vDevice.DestroyCommandBufferList(pCommandBufferList);
			vDevice.DestroyCommandBufferList(pVolatileCommandBufferList);
			vDevice.DestroyCommandBufferList(std::move(pSecondaryCommandBuffer));

			vDevice.DestroyImage(std::move(pColorImage));
			vDevice.DestroyImage(std::move(pDepthImage));

			for (auto pResult : pResults)
				vDevice.DestroyImage(pResult);

			pResults.clear();
		}

		VkFramebuffer VulkanPointShadowMap::GetFrameBuffer(UI32 index) const
		{
			return vRenderTarget.vFrameBuffers[index];
		}

		FColor4D VulkanPointShadowMap::GetClearColor() const
		{
			return FColor4D(pClearValues[0].color.float32[0], pClearValues[0].color.float32[1], pClearValues[0].color.float32[2], pClearValues[0].color.float32[3]);
		}

		void VulkanPointShadowMap::SetClearColor(const FColor4D& newColor)
		{
			pClearValues[0].color.float32[0] = newColor.mRed;
			pClearValues[0].color.float32[1] = newColor.mGreen;
			pClearValues[0].color.float32[2] = newColor.mBlue;
			pClearValues[0].color.float32[3] = newColor.mAlpha;
		}
	}
}