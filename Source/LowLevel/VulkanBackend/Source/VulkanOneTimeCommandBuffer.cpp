// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanOneTimeCommandBuffer.h"
#include "VulkanBackend/VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanOneTimeCommandBuffer::VulkanOneTimeCommandBuffer(const VulkanDevice* pDevice) : pDevice(const_cast<VulkanDevice*>(pDevice))
		{
			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = VK_NULL_HANDLE;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = this->pDevice->GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(pDevice->CreateCommanPool(&vPoolCI, &vPool), "Failed to create command pool!")

				VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandPool = vPool;
			vAI.commandBufferCount = 1;

			FLINT_VK_ASSERT(pDevice->AllocateCommandBuffers(&vAI, { vBuffer }), "Failed to allocate command buffers!");

			VkCommandBufferBeginInfo vBI = {};
			vBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBI.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			FLINT_VK_ASSERT(vkBeginCommandBuffer(vBuffer, &vBI), "Failed to begin command buffer recording!");
		}

		VulkanOneTimeCommandBuffer::~VulkanOneTimeCommandBuffer()
		{
			vkEndCommandBuffer(vBuffer);

			VkSubmitInfo vSI = {};
			vSI.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSI.commandBufferCount = 1;
			vSI.pCommandBuffers = &vBuffer;

			vkQueueSubmit(pDevice->GetGraphcisQueue(), 1, &vSI, VK_NULL_HANDLE);
			vkQueueWaitIdle(pDevice->GetTransferQueue());

			pDevice->FreeComandBuffers(vPool, { vBuffer });
			pDevice->DestroyCommandPool(vPool);
		}

		VulkanOneTimeCommandBuffer::operator VkCommandBuffer() const
		{
			return vBuffer;
		}

		VulkanOneTimeCommandBuffer::operator VkCommandPool() const
		{
			return vPool;
		}
	}
}