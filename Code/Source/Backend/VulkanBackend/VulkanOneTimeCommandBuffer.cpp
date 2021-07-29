// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanOneTimeCommandBuffer::VulkanOneTimeCommandBuffer(const VulkanDevice& device) : vDevice(const_cast<VulkanDevice&>(device))
		{
			FLINT_SETUP_PROFILER();

			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = 0;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = vDevice.GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vPoolCI, nullptr, &vPool));

			VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandPool = vPool;
			vAI.commandBufferCount = 1;

			FLINT_VK_ASSERT(vkAllocateCommandBuffers(vDevice.GetLogicalDevice(), &vAI, &vBuffer));

			VkCommandBufferBeginInfo vBI = {};
			vBI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			//vBI.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			FLINT_VK_ASSERT(vkBeginCommandBuffer(vBuffer, &vBI));
		}

		VulkanOneTimeCommandBuffer::~VulkanOneTimeCommandBuffer()
		{
			FLINT_SETUP_PROFILER();

			FLINT_VK_ASSERT(vkEndCommandBuffer(vBuffer));

			VkSubmitInfo vSI = {};
			vSI.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSI.commandBufferCount = 1;
			vSI.pCommandBuffers = &vBuffer;

			VkFenceCreateInfo vFCI = {};
			vFCI.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vFCI.pNext = VK_NULL_HANDLE;
			vFCI.flags = 0;

			VkFence vFence = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(vkCreateFence(vDevice.GetLogicalDevice(), &vFCI, nullptr, &vFence));

			FLINT_VK_ASSERT(vkQueueSubmit(vDevice.GetQueue().vTransferQueue, 1, &vSI, vFence));
			FLINT_VK_ASSERT(vkWaitForFences(vDevice.GetLogicalDevice(), 1, &vFence, VK_TRUE, UINT64_MAX));
			vkDestroyFence(vDevice.GetLogicalDevice(), vFence, nullptr);

			vkFreeCommandBuffers(vDevice.GetLogicalDevice(), vPool, 1, &vBuffer);
			vkDestroyCommandPool(vDevice.GetLogicalDevice(), vPool, nullptr);
		}
	}
}