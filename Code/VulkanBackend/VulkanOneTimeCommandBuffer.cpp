// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include <volk.h>
export module Flint.VulkanBackend.VulkanOneTimeCommandBuffer;

import Flint.VulkanBackend.VulkanDevice;

export namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanOneTimeCommandBuffer {
		public:
			VulkanOneTimeCommandBuffer(const VulkanDevice& device);
			~VulkanOneTimeCommandBuffer();

			operator VkCommandBuffer() const { return vBuffer; }
			operator VkCommandPool() const { return vPool; }

			VkCommandBuffer GetBuffer() const { return vBuffer; }
			VkCommandPool GetPool() const { return vPool; }

		private:
			VulkanDevice& vDevice;

			VkCommandBuffer vBuffer = VK_NULL_HANDLE;
			VkCommandPool vPool = VK_NULL_HANDLE;
		};
	}
}

module: private;

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanOneTimeCommandBuffer::VulkanOneTimeCommandBuffer(const VulkanDevice& device) : vDevice(const_cast<VulkanDevice&>(device))
		{
			OPTICK_EVENT();

			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = 0;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = vDevice.GetQueue().mTransferFamily.value();

			Utilities::CheckResult(vDevice.GetDeviceTable().vkCreateCommandPool(vDevice.GetLogicalDevice(), &vPoolCI, nullptr, &vPool));

			VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandPool = vPool;
			vAI.commandBufferCount = 1;

			Utilities::CheckResult(vDevice.GetDeviceTable().vkAllocateCommandBuffers(vDevice.GetLogicalDevice(), &vAI, &vBuffer));

			VkCommandBufferBeginInfo vBI = {};
			vBI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			//vBI.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			Utilities::CheckResult(vDevice.GetDeviceTable().vkBeginCommandBuffer(vBuffer, &vBI));
		}

		VulkanOneTimeCommandBuffer::~VulkanOneTimeCommandBuffer()
		{
			OPTICK_EVENT();

			Utilities::CheckResult(vDevice.GetDeviceTable().vkEndCommandBuffer(vBuffer));

			VkSubmitInfo vSI = {};
			vSI.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSI.commandBufferCount = 1;
			vSI.pCommandBuffers = &vBuffer;

			VkFenceCreateInfo vFCI = {};
			vFCI.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vFCI.pNext = VK_NULL_HANDLE;
			vFCI.flags = 0;

			VkFence vFence = VK_NULL_HANDLE;
			Utilities::CheckResult(vDevice.GetDeviceTable().vkCreateFence(vDevice.GetLogicalDevice(), &vFCI, nullptr, &vFence));

			Utilities::CheckResult(vDevice.GetDeviceTable().vkQueueSubmit(vDevice.GetQueue().vTransferQueue, 1, &vSI, vFence));
			Utilities::CheckResult(vDevice.GetDeviceTable().vkWaitForFences(vDevice.GetLogicalDevice(), 1, &vFence, VK_TRUE, std::numeric_limits<uint64>::max()));
			Utilities::CheckResult(vDevice.GetDeviceTable().vkQueueWaitIdle(vDevice.GetQueue().vTransferQueue));
			vDevice.GetDeviceTable().vkDestroyFence(vDevice.GetLogicalDevice(), vFence, nullptr);

			vDevice.GetDeviceTable().vkFreeCommandBuffers(vDevice.GetLogicalDevice(), vPool, 1, vSI.pCommandBuffers);
			vDevice.GetDeviceTable().vkDestroyCommandPool(vDevice.GetLogicalDevice(), vPool, nullptr);
		}
	}
}