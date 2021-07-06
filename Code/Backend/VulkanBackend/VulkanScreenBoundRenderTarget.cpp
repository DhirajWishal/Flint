// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanScreenBoundRenderTarget.hpp"
#include "VulkanCommandBufferList.hpp"

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

			vRenderTarget.CreateRenderPass({ pSwapChain, pColorBuffer, pDepthBuffer }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			vRenderTarget.CreateFrameBuffer({ pSwapChain, pColorBuffer, pDepthBuffer }, extent, bufferCount);
			vRenderTarget.CreateSyncObjects(bufferCount);
		}

		void VulkanScreenBoundRenderTarget::BeginFrame()
		{
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
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			auto& vCommandBufferList = pCommandBufferList->StaticCast<VulkanCommandBufferList>();
			auto& vDisplay = pDisplay->StaticCast<VulkanDisplay>();

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
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			auto& vDisplay = pDisplay->StaticCast<VulkanDisplay>();
			mExtent = vDisplay.GetExtent();

			pSwapChain->Recreate(mExtent);
			pColorBuffer->Recreate(mExtent);
			pDepthBuffer->Recreate(mExtent);

			vRenderTarget.DestroyRenderPass();
			vRenderTarget.DestroyFrameBuffers();

			vRenderTarget.CreateRenderPass({ pSwapChain, pColorBuffer, pDepthBuffer }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			vRenderTarget.CreateFrameBuffer({ pSwapChain, pColorBuffer, pDepthBuffer }, mExtent, mBufferCount);
		}
	}
}