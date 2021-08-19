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
			pResults.push_back(pDevice->CreateImage(ImageType::CUBEMAP, ImageUsage::GRAPHICS, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), PixelFormat::R32_SFLOAT, 6, 1, nullptr));

			pColorImage = std::make_unique<VulkanImage>(pDevice, ImageType::DIMENSIONS_2, ImageUsage::COLOR, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), PixelFormat::R32_SFLOAT, 1, 1, nullptr);
			pDepthImage = std::make_unique<VulkanImage>(pDevice, ImageType::DIMENSIONS_2, ImageUsage::DEPTH, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), PixelFormat::D16_SINT, 1, 1, nullptr);

			vRenderTarget.CreateRenderPass({ pColorImage.get(), pDepthImage.get() }, VK_PIPELINE_BIND_POINT_GRAPHICS, {});
			vRenderTarget.CreateFrameBuffer({ pColorImage.get(), pDepthImage.get() }, mExtent, bufferCount * 6);
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

			// Recreate the command buffers if the render target is altered.
			if (bIsAltered)
			{
				PrepareCommandBuffers();
				bIsAltered = false;
			}

			if (pScreenBoundRenderTarget)
			{
				VulkanScreenBoundRenderTarget& vScreenBoundRenderTarget = pScreenBoundRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();
				auto& vPrimaryCommandBufferList = pCommandBufferList->StaticCast<VulkanCommandBufferList>();

				mFrameIndex = vScreenBoundRenderTarget.GetFrameIndex();
				vScreenBoundRenderTarget.AddSubmitCommandBuffer(vPrimaryCommandBufferList.GetCommandBuffer(mFrameIndex));
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

			if (pSecondaryCommandBuffer)
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

		void VulkanPointShadowMap::PrepareCommandBuffers()
		{
			UI64 drawInstanceCount = 0;

			VulkanImage& vResultImage = pResults.back()->StaticCast<VulkanImage>();

			// Begin the command buffer.
			auto& vPrimaryCommandBufferList = pCommandBufferList->StaticCast<VulkanCommandBufferList>();

			// Bind the draw instances.
			for (UI32 frame = 0; frame < mBufferCount; frame++)
			{
				vPrimaryCommandBufferList.BeginBufferRecording(frame);

				for (UI64 itr = 0; itr < mDrawInstanceOrder.size(); itr++)
				{
					const auto drawOrder = mDrawInstanceOrder[itr];
					const auto drawInstanceMap = mDrawInstanceMaps[itr];

					for (const auto store : drawOrder)
					{
						FLINT_SETUP_PROFILER();

						vPrimaryCommandBufferList.BindVertexBuffer(store->GetVertexBuffer());
						vPrimaryCommandBufferList.BindIndexBuffer(store->GetIndexBuffer(), store->GetIndexSize());

						auto& pipelines = drawInstanceMap.at(store);

						// Iterate through the pipelines.
						for (auto& pipeline : pipelines)
						{
							pipeline->PrepareResourcesToDraw();
							vPrimaryCommandBufferList.BindGraphicsPipeline(pipeline);

							const auto drawData = pipeline->GetDrawData();
							const auto drawDataIndex = pipeline->GetCurrentDrawIndex();

							// Validate the draw data count.
							if (drawData.size() % 6 != 0)
								FLINT_THROW_BACKEND_ERROR("Invalid draw count! When drawing objects to the point shadow map make sure that you have 6 instances of the same draw data so that the render target can draw each into a separate cube map face.");

							for (UI8 face = 0; face < 6; face++)
							{
								vPrimaryCommandBufferList.VulkanBindRenderTarget(pThisRenderTarget, GetFrameBuffer((frame * 6) + face));

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

									vPrimaryCommandBufferList.BindDrawResources(pipeline, draw.pResourceMap);
									vPrimaryCommandBufferList.BindDynamicStates(pipeline, draw.pDynamicStates);
									vPrimaryCommandBufferList.IssueDrawCall(draw.mVertexOffset, draw.mVertexCount, draw.mIndexOffset, draw.mIndexCount);

									counter = ++counter % 6;
								}

								vPrimaryCommandBufferList.UnbindRenderTarget();

								// Copy the image to the cube map.
								pColorImage->SetImageLayout(vPrimaryCommandBufferList.GetCurrentCommandBuffer(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
								vResultImage.SetImageLayout(vPrimaryCommandBufferList.GetCurrentCommandBuffer(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, face, 1);

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
									vPrimaryCommandBufferList.GetCurrentCommandBuffer(),
									pColorImage->GetImage(),
									VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
									vResultImage.GetImage(),
									VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
									1,
									&vImageCopy);

								pColorImage->SetImageLayout(vPrimaryCommandBufferList.GetCurrentCommandBuffer(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
								vResultImage.SetImageLayout(vPrimaryCommandBufferList.GetCurrentCommandBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, face, 1);
							}
						}
					}
				}

				vPrimaryCommandBufferList.EndBufferRecording();
			}

			mFrameIndex = 0;
		}
	}
}