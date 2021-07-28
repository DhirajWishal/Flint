// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanCommandBufferList.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanScreenBoundRenderTarget::VulkanScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FExtent2D& extent, const UI32 bufferCount)
			: ScreenBoundRenderTarget(pDevice, pDisplay, extent, bufferCount, pDevice->CreatePrimaryCommandBufferList(bufferCount)), vRenderTarget(pDevice->StaticCast<VulkanDevice>())
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			auto& vDisplay = pDisplay->StaticCast<VulkanDisplay>();

			pSwapChain = new VulkanSwapChain(vDevice, vDisplay, extent, bufferCount);
			pColorBuffer = new VulkanColorBuffer(vDevice, extent, bufferCount, pSwapChain->GetFormat());
			pDepthBuffer = new VulkanDepthBuffer(vDevice, extent, bufferCount);

			vRenderTarget.CreateRenderPass({ pColorBuffer, pDepthBuffer, pSwapChain }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			vRenderTarget.CreateFrameBuffer({ pColorBuffer, pDepthBuffer, pSwapChain }, extent, bufferCount);
			vRenderTarget.CreateSyncObjects(bufferCount);

			// Setup default clear color values.
			pClearValues[0].color.float32[0] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[1] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[2] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[3] = 1.0f;
			pClearValues[1].depthStencil.depth = 1.0f;
			pClearValues[1].depthStencil.stencil = 0;
		}

		void VulkanScreenBoundRenderTarget::PrepareStaticResources()
		{
			VulkanCommandBufferList& vCommandBufferList = pCommandBufferList->StaticCast<VulkanCommandBufferList>();
			std::shared_ptr<ScreenBoundRenderTarget> pThisRenderTarget = this->shared_from_this();

			for (UI32 iter = 0; iter < vCommandBufferList.GetBufferCount(); iter++)
			{
				vCommandBufferList.BeginNextBufferRecording();
				vCommandBufferList.BindRenderTarget(pThisRenderTarget);

				// bind resources here. TODO

				vCommandBufferList.UnbindRenderTarget();
				vCommandBufferList.EndBufferRecording();
			}
		}

		void VulkanScreenBoundRenderTarget::BeginFrame()
		{
			// Skip if the screen is reported to be 0 in width or height.
			if (bShouldSkip)
				return;

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vkWaitForFences(vDevice.GetLogicalDevice(), 1, &vRenderTarget.vInFlightFences[mFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()));
			//FLINT_VK_ASSERT(pvDevice->ResetFences({ vInFlightFences[mFrameIndex] }), "Failed to reset fence!")

			VkResult result = vkAcquireNextImageKHR(vDevice.GetLogicalDevice(), pSwapChain->GetSwapChain(), std::numeric_limits<UI32>::max(), vRenderTarget.vImageAvailables[mFrameIndex], VK_NULL_HANDLE, &mImageIndex);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
				Recreate();
			else FLINT_VK_ASSERT(result);
		}

		void VulkanScreenBoundRenderTarget::Update()
		{
		}

		void VulkanScreenBoundRenderTarget::SubmitFrame()
		{
			auto& vDisplay = pDisplay->StaticCast<VulkanDisplay>();

			// Skip if the screen is reported to be 0 in width or height.
			if (bShouldSkip)
			{
				Recreate();
				return;
			}

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			auto& vCommandBufferList = pCommandBufferList->StaticCast<VulkanCommandBufferList>();

			if (vRenderTarget.vImagesInFlightFences[mImageIndex] != VK_NULL_HANDLE)
				FLINT_VK_ASSERT(vkWaitForFences(vDevice.GetLogicalDevice(), 1, &vRenderTarget.vImagesInFlightFences[mImageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()));

			vRenderTarget.vImagesInFlightFences[mImageIndex] = vRenderTarget.vInFlightFences[mFrameIndex];

			VkSemaphoreWaitFlags vWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkCommandBuffer vCommandBuffer[1] = { vCommandBufferList.GetCommandBuffer(mFrameIndex) };

			VkSubmitInfo vSI = {};
			vSI.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSI.pNext = VK_NULL_HANDLE;
			vSI.commandBufferCount = 1;
			vSI.pCommandBuffers = vCommandBuffer;
			vSI.waitSemaphoreCount = 1;
			vSI.pWaitSemaphores = &vRenderTarget.vImageAvailables[mFrameIndex];
			vSI.signalSemaphoreCount = 1;
			vSI.pSignalSemaphores = &vRenderTarget.vRenderFinishes[mFrameIndex];
			vSI.pWaitDstStageMask = &vWaitStage;

			FLINT_VK_ASSERT(vkResetFences(vDevice.GetLogicalDevice(), 1, &vRenderTarget.vInFlightFences[mFrameIndex]));
			FLINT_VK_ASSERT(vkQueueSubmit(vDevice.GetQueue().vGraphicsQueue, 1, &vSI, vRenderTarget.vInFlightFences[mFrameIndex]));

			VkPresentInfoKHR vPI = {};
			vPI.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			vPI.pNext = VK_NULL_HANDLE;
			vPI.waitSemaphoreCount = 1;
			vPI.pWaitSemaphores = &vRenderTarget.vRenderFinishes[mFrameIndex];
			vPI.swapchainCount = 1;

			VkSwapchainKHR vSwapChain = pSwapChain->GetSwapChain();
			vPI.pSwapchains = &vSwapChain;
			vPI.pImageIndices = &mImageIndex;

			IncrementFrameIndex();
			VkResult result = vkQueuePresentKHR(vDevice.GetQueue().vTransferQueue, &vPI);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vDisplay.IsDisplayResized())
			{
				vDisplay.ToggleResize();
				Recreate();
			}
			else FLINT_VK_ASSERT(result);

			FLINT_VK_ASSERT(vkQueueWaitIdle(vDevice.GetQueue().vTransferQueue));
		}

		void VulkanScreenBoundRenderTarget::Terminate()
		{
			vRenderTarget.Terminate();

			pSwapChain->Terminate();
			delete pSwapChain;

			pColorBuffer->Terminate();
			delete pColorBuffer;

			pDepthBuffer->Terminate();
			delete pDepthBuffer;

			pDevice->DestroyCommandBufferList(pCommandBufferList);
		}

		void VulkanScreenBoundRenderTarget::Recreate()
		{
			FExtent2D newExtent = pDisplay->GetExtent();

			// Wait while the window contains the right width and height. 
			// Until the window contains a valid size, it will skip draw calls.
			if (newExtent.mWidth == 0 || newExtent.mHeight == 0)
			{
				bShouldSkip = true;
				return;
			}

			mExtent = newExtent;
			bShouldSkip = false;

			pDevice->WaitIdle();
			pCommandBufferList->ClearBuffers();

			vRenderTarget.DestroyRenderPass();
			vRenderTarget.DestroyFrameBuffers();

			pSwapChain->Recreate(mExtent);
			pColorBuffer->Recreate(mExtent);
			pDepthBuffer->Recreate(mExtent);

			vRenderTarget.CreateRenderPass({ pColorBuffer, pDepthBuffer, pSwapChain }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			vRenderTarget.CreateFrameBuffer({ pColorBuffer, pDepthBuffer, pSwapChain }, mExtent, mBufferCount);

			PrepareStaticResources();
			vRenderTarget.vInFlightFences.resize(vRenderTarget.vInFlightFences.size(), VK_NULL_HANDLE);

			mImageIndex = 0, mFrameIndex = 0;
		}

		FColor4D VulkanScreenBoundRenderTarget::GetClearColor() const
		{
			return FColor4D(pClearValues[0].color.float32[0], pClearValues[0].color.float32[1], pClearValues[0].color.float32[2], pClearValues[0].color.float32[3]);
		}

		void VulkanScreenBoundRenderTarget::SetClearColor(const FColor4D& newColor)
		{
			pClearValues[0].color.float32[0] = newColor.mRed;
			pClearValues[0].color.float32[1] = newColor.mGreen;
			pClearValues[0].color.float32[2] = newColor.mBlue;
			pClearValues[0].color.float32[3] = newColor.mAlpha;
		}
	}
}