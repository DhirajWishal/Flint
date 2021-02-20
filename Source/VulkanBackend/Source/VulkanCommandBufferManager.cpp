// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBufferManager.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/RenderTargets/VulkanScreenBoundRenderTargetS.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanCommandBufferManager::CreateBuffers(Backend::Device* pDevice, UI32 count)
		{
			this->pDevice = pDevice;
			ResolveCommandBuffers(count);
			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();

			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = VK_NULL_HANDLE;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = pvDevice->GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(pvDevice->CreateCommanPool(&vPoolCI, &vCommandPool), "Failed to create command pool!")

				VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandBufferCount = count;
			vAI.commandPool = vCommandPool;

			vCommandBuffers.resize(count);
			FLINT_VK_ASSERT(pvDevice->AllocateCommandBuffers(&vAI, vCommandBuffers), "Failed to allocate command buffers!")

				InitializeSyncObjects(count);
		}

		void VulkanCommandBufferManager::Terminate()
		{
			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			pvDevice->FreeComandBuffers(vCommandPool, vCommandBuffers);
			pvDevice->DestroyCommandPool(vCommandPool);

			vCommandBuffers.clear();
			TerminateSyncObjects();
		}

		void VulkanCommandBufferManager::BeginBufferRecording(UI32 index)
		{
			VkCommandBufferBeginInfo vBI = {};
			vBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBI.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBI.pNext = VK_NULL_HANDLE;
			vBI.pInheritanceInfo = VK_NULL_HANDLE;

			FLINT_VK_ASSERT(vkBeginCommandBuffer(vCommandBuffers[index], &vBI), "Failed to begin command buffer recording!")
		}

		void VulkanCommandBufferManager::EndBufferRecording(UI32 index)
		{
			FLINT_VK_ASSERT(vkEndCommandBuffer(vCommandBuffers[index]), "Failed to end command buffer recording!")
		}

		I32 VulkanCommandBufferManager::BeginCommandExecution(UI32 frameIndex, Backend::ScreenBoundRenderTarget* pRenderTarget)
		{
			UI32 index = 0;
			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();

			FLINT_VK_ASSERT(vkWaitForFences(pvDevice->GetLogicalDevice(), 1, &vInFlightFences[frameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()), "Failed to wait for fence!")
			VkResult result = vkAcquireNextImageKHR(pvDevice->GetLogicalDevice(), pRenderTarget->Derive<VulkanScreenBoundRenderTargetS>()->GetSwapChain(), std::numeric_limits<UI32>::max(), vImageAvailables[frameIndex], vInFlightFences[frameIndex], &index);

			if (result == VkResult::VK_ERROR_OUT_OF_DATE_KHR)
				index = -1;
			else if (result == VkResult::VK_SUBOPTIMAL_KHR)
				index = -2;

			return static_cast<I32>(index);
		}

		void VulkanCommandBufferManager::SubmitCommand(UI32 index, Backend::ScreenBoundRenderTarget* pRenderTarget)
		{
			VkSemaphoreWaitFlags vWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			VkSubmitInfo vSI = {};
			vSI.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSI.pNext = VK_NULL_HANDLE;
			vSI.commandBufferCount = 1;
			vSI.pCommandBuffers = &vCommandBuffers[index];
			vSI.waitSemaphoreCount = 1;
			vSI.pWaitSemaphores = &vImageAvailables[index];
			vSI.signalSemaphoreCount = 1;
			vSI.pSignalSemaphores = &vRenderFinishes[index];
			vSI.pWaitDstStageMask = &vWaitStage;

			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			FLINT_VK_ASSERT(vkResetFences(pvDevice->GetLogicalDevice(), 1, &vInFlightFences[index]), "Failed to reset fence!")

				FLINT_VK_ASSERT(pvDevice->SubmitQueue(pvDevice->GetGraphicsQueue(), { vSI }, vInFlightFences[index]), "Failed to submit queue!")

				VkPresentInfoKHR vPI = {};
			vPI.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			vPI.pNext = VK_NULL_HANDLE;
			vPI.waitSemaphoreCount = 1;
			vPI.pWaitSemaphores = &vRenderFinishes[index];
			vPI.swapchainCount = 1;

			VkSwapchainKHR vSwapChain = pRenderTarget->Derive<VulkanScreenBoundRenderTargetS>()->GetSwapChain();
			vPI.pSwapchains = &vSwapChain;
			vPI.pImageIndices = &index;

			FLINT_VK_ASSERT(vkQueuePresentKHR(pvDevice->GetTransferQueue(), &vPI), "Failed to present queue!")
		}

		void VulkanCommandBufferManager::InitializeSyncObjects(UI32 count)
		{
			vImageAvailables.resize(count);
			vRenderFinishes.resize(count);
			vInFlightFences.resize(count);

			VkSemaphoreCreateInfo vSCI = {};
			vSCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo VFCI = {};
			VFCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			VFCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			FLINT_VK_ASSERT(pvDevice->CreateSemaphores(&vSCI, vImageAvailables), "Failed to create Image Available semaphores!")
				FLINT_VK_ASSERT(pvDevice->CreateSemaphores(&vSCI, vRenderFinishes), "Failed to create Render Finished semaphores!")
				FLINT_VK_ASSERT(pvDevice->CreateFences(&VFCI, vInFlightFences), "Failed to create In Flight fences!")
		}

		void VulkanCommandBufferManager::TerminateSyncObjects()
		{
			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			pvDevice->DestroySemaphores(vImageAvailables);
			pvDevice->DestroySemaphores(vRenderFinishes);
			pvDevice->DestroyFences(vInFlightFences);
		}
	}
}