// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanOneTimeCommandBuffer::VulkanOneTimeCommandBuffer(const VulkanDevice& device) : m_vDevice(const_cast<VulkanDevice&>(device))
		{
			OPTICK_EVENT();

			VkCommandPoolCreateInfo m_vPoolCI = {};
			m_vPoolCI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			m_vPoolCI.flags = 0;
			m_vPoolCI.pNext = VK_NULL_HANDLE;
			m_vPoolCI.queueFamilyIndex = m_vDevice.GetQueue().m_TransferFamily.value();

			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateCommandPool(m_vDevice.GetLogicalDevice(), &m_vPoolCI, nullptr, &m_vPool));

			VkCommandBufferAllocateInfo m_vAI = {};
			m_vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			m_vAI.pNext = VK_NULL_HANDLE;
			m_vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			m_vAI.commandPool = m_vPool;
			m_vAI.commandBufferCount = 1;

			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkAllocateCommandBuffers(m_vDevice.GetLogicalDevice(), &m_vAI, &m_vBuffer));

			VkCommandBufferBeginInfo m_vBI = {};
			m_vBI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			//m_vBI.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkBeginCommandBuffer(m_vBuffer, &m_vBI));
		}

		VulkanOneTimeCommandBuffer::~VulkanOneTimeCommandBuffer()
		{
			OPTICK_EVENT();

			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkEndCommandBuffer(m_vBuffer));

			VkSubmitInfo m_vSI = {};
			m_vSI.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
			m_vSI.commandBufferCount = 1;
			m_vSI.pCommandBuffers = &m_vBuffer;

			VkFenceCreateInfo m_vFCI = {};
			m_vFCI.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			m_vFCI.pNext = VK_NULL_HANDLE;
			m_vFCI.flags = 0;

			VkFence m_vFence = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateFence(m_vDevice.GetLogicalDevice(), &m_vFCI, nullptr, &m_vFence));

			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkQueueSubmit(m_vDevice.GetQueue().m_vTransferQueue, 1, &m_vSI, m_vFence));
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkWaitForFences(m_vDevice.GetLogicalDevice(), 1, &m_vFence, VK_TRUE, std::numeric_limits<uint64_t>::max()));
			m_vDevice.GetDeviceTable().vkDestroyFence(m_vDevice.GetLogicalDevice(), m_vFence, nullptr);

			m_vDevice.GetDeviceTable().vkFreeCommandBuffers(m_vDevice.GetLogicalDevice(), m_vPool, 1, m_vSI.pCommandBuffers);
			m_vDevice.GetDeviceTable().vkDestroyCommandPool(m_vDevice.GetLogicalDevice(), m_vPool, nullptr);
		}
	}
}