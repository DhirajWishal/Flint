// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanImage.hpp"
#include "GeometryStore.hpp"

#include <iostream>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanScreenBoundRenderTarget::VulkanScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, SwapChainPresentMode presentMode)
			: ScreenBoundRenderTarget(pDevice, pDisplay, extent, bufferCount, imageAttachments, presentMode), vRenderTarget(pDevice->StaticCast<VulkanDevice>())
		{
			FLINT_SETUP_PROFILER();

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			auto& vDisplay = pDisplay->StaticCast<VulkanDisplay>();

			pSwapChain = std::make_unique<VulkanSwapChain>(pDevice, pDisplay, bufferCount, presentMode);

			vDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[0].dstSubpass = 0;
			vDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			vDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			vDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			vDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			vDependencies[1].srcSubpass = 0;
			vDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			vDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			vDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			vDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			std::vector<VulkanRenderTargetAttachmentInterface*> pAttachmentInferfaces;
			pAttachmentInferfaces.reserve(mAttachments.size() + 1);
			for (const auto attachment : mAttachments)
				pAttachmentInferfaces.push_back(&attachment.pImage->StaticCast<VulkanImage>());

			pAttachmentInferfaces.push_back(pSwapChain.get());

			vRenderTarget.CreateRenderPass(pAttachmentInferfaces, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
			vRenderTarget.CreateFrameBuffer(pAttachmentInferfaces, extent, bufferCount);
			vRenderTarget.CreateSyncObjects(bufferCount);
		}

		void VulkanScreenBoundRenderTarget::PresentToDisplay()
		{
			auto nextImage = pSwapChain->AcquireNextImage();
			if (nextImage.bShouldRecreate)
				Recreate();

			VkResult vResult = vkQueuePresentKHR(pDevice->StaticCast<VulkanDevice>().GetQueue().vTransferQueue, pSwapChain->PrepareToPresent());
			if (vResult == VK_ERROR_OUT_OF_DATE_KHR || vResult == VK_SUBOPTIMAL_KHR)
				Recreate();
			else FLINT_VK_ASSERT(vResult);

			IncrementFrameIndex();
		}

		void VulkanScreenBoundRenderTarget::Terminate()
		{
			vRenderTarget.Terminate();
			pSwapChain->Terminate();

			bIsTerminated = true;
		}

		void VulkanScreenBoundRenderTarget::Recreate()
		{
			FLINT_SETUP_PROFILER();

			FBox2D newExtent = pDisplay->GetExtent();
			if (newExtent.IsZero())
			{
				bShouldSkip = true;
				return;
			}

			mExtent = newExtent;
			bShouldSkip = false;

			pDevice->WaitIdle();

			vRenderTarget.DestroyRenderPass();
			vRenderTarget.DestroyFrameBuffers();

			pSwapChain->Recreate();
			for (auto attachment : mAttachments)
				attachment.pImage->StaticCast<VulkanImage>().Recreate(mExtent);

			std::vector<VulkanRenderTargetAttachmentInterface*> pAttachmentInferfaces;
			pAttachmentInferfaces.reserve(mAttachments.size() + 1);
			for (const auto attachment : mAttachments)
				pAttachmentInferfaces.push_back(&attachment.pImage->StaticCast<VulkanImage>());

			pAttachmentInferfaces.push_back(pSwapChain.get());

			vRenderTarget.CreateRenderPass(pAttachmentInferfaces, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
			vRenderTarget.CreateFrameBuffer(pAttachmentInferfaces, mExtent, mBufferCount);

			vRenderTarget.vInFlightFences.resize(vRenderTarget.vInFlightFences.size(), VK_NULL_HANDLE);
			mFrameIndex = 0;
		}

		const VkFramebuffer VulkanScreenBoundRenderTarget::PrepareAndGetFramebuffer()
		{
			const auto frameBuffer = vRenderTarget.vFrameBuffers[mFrameIndex];
			IncrementFrameIndex();

			return frameBuffer;
		}
		
		std::vector<VkClearValue> VulkanScreenBoundRenderTarget::GetClearScreenValues() const
		{
			std::vector<VkClearValue> vClearValues;
			vClearValues.reserve(mAttachments.size());
			for (const auto attachment : mAttachments)
			{
				VkClearValue vClearValue = {};
				vClearValue.color.float32[0] = attachment.mClearColor.mRed;
				vClearValue.color.float32[1] = attachment.mClearColor.mGreen;
				vClearValue.color.float32[2] = attachment.mClearColor.mBlue;
				vClearValue.color.float32[3] = attachment.mClearColor.mAlpha;

				vClearValue.depthStencil.depth = attachment.mDepthClearValue.mDepth;
				vClearValue.depthStencil.stencil = attachment.mDepthClearValue.mStencil;
				vClearValues.push_back(vClearValue);
			}

			return vClearValues;
		}
	}
}