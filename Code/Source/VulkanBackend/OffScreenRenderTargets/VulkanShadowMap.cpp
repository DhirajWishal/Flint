// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/OffScreenRenderTargets/VulkanShadowMap.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanImage.hpp"
#include "GeometryStore.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanShadowMap::VulkanShadowMap(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, UI32 threadCount)
			: VulkanOffScreenRenderTarget(pDevice, OffScreenRenderTargetType::SHADOW_MAP, extent, bufferCount, threadCount)
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			pSecondaryCommandBuffer = std::make_unique<VulkanCommandBufferList>(pDevice, bufferCount, pCommandBufferList);
			pResults.push_back(pDevice->CreateImage(ImageType::DIMENSIONS_2, ImageUsage::DEPTH, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), PixelFormat::D16_SINT, 1, 1, nullptr));

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

			vRenderTarget.CreateRenderPass({ &pResults.back()->StaticCast<VulkanImage>() }, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
			vRenderTarget.CreateFrameBuffer({ &pResults.back()->StaticCast<VulkanImage>() }, extent, bufferCount);
			vRenderTarget.CreateSyncObjects(bufferCount);

			// Setup default clear color values.
			vClearValue.depthStencil.depth = 1.0f;
			vClearValue.depthStencil.stencil = 0;
		}

		void VulkanShadowMap::Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget)
		{
			if (!pThisRenderTarget)
				pThisRenderTarget = shared_from_this();

			if (pScreenBoundRenderTarget)
			{
				VulkanScreenBoundRenderTarget& vScreenBoundRenderTarget = pScreenBoundRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();

				// Begin the command buffer.
				auto& vPrimaryCommandBufferList = pCommandBufferList->StaticCast<VulkanCommandBufferList>();
				mFrameIndex = vScreenBoundRenderTarget.GetFrameIndex();

				// Iterate through frame buffers in a cluster.
				vInheritInfo.renderPass = vRenderTarget.vRenderPass;
				vInheritInfo.framebuffer = GetFrameBuffer(mFrameIndex);

				vPrimaryCommandBufferList.BeginBufferRecording(mFrameIndex);
				vPrimaryCommandBufferList.VulkanBindRenderTargetSecondary(pThisRenderTarget, vInheritInfo.framebuffer);

				// Begin secondary command buffers.
				auto& vCommandBufferList = *pSecondaryCommandBuffer;
				vCommandBufferList.VulkanBeginSecondaryCommandBuffer(mFrameIndex, &vInheritInfo);

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

				vPrimaryCommandBufferList.AddSecondaryCommandBuffer(vCommandBufferList.GetCurrentCommandBuffer());
				vPrimaryCommandBufferList.ExecuteSecondaryCommands();
				vPrimaryCommandBufferList.UnbindRenderTarget();
				vPrimaryCommandBufferList.EndBufferRecording();

				vScreenBoundRenderTarget.AddSubmitCommandBuffer(vPrimaryCommandBufferList.GetCurrentCommandBuffer());
			}
			else
			{
				// TODO
				IncrementFrameIndex();
			}
		}

		void VulkanShadowMap::Terminate()
		{
			vRenderTarget.Terminate();

			pCommandBufferList->Terminate();
			pSecondaryCommandBuffer->Terminate();

			for (auto pResult : pResults)
				pResult->Terminate();

			pResults.clear();
			bIsTerminated = true;
		}

		VkFramebuffer VulkanShadowMap::GetFrameBuffer(UI32 index) const
		{
			return vRenderTarget.vFrameBuffers[index];
		}

		FColor4D VulkanShadowMap::GetClearColor() const
		{
			return FColor4D(vClearValue.color.float32[0], vClearValue.color.float32[1], vClearValue.color.float32[2], vClearValue.color.float32[3]);
		}

		void VulkanShadowMap::SetClearColor(const FColor4D& newColor)
		{
			vClearValue.color.float32[0] = newColor.mRed;
			vClearValue.color.float32[1] = newColor.mGreen;
			vClearValue.color.float32[2] = newColor.mBlue;
			vClearValue.color.float32[3] = newColor.mAlpha;
		}
	}
}