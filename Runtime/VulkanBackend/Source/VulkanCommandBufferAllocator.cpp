// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBufferAllocator.hpp"
#include "VulkanBackend/VulkanCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBufferAllocator::VulkanCommandBufferAllocator(VulkanDevice* m_pDevice, const uint32_t bufferCount)
			: CommandBufferAllocator(m_pDevice, bufferCount)
		{
			OPTICK_EVENT();

			VkCommandPoolCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			m_vCreateInfo.queueFamilyIndex = m_pDevice->GetQueue().m_TransferFamily.value();

			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkCreateCommandPool(m_pDevice->GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vCommandPool));
		}

		VulkanCommandBufferAllocator::VulkanCommandBufferAllocator(VulkanDevice* m_pDevice, VulkanCommandBufferAllocator* pParent, const uint32_t bufferCount)
			: CommandBufferAllocator(m_pDevice, pParent, bufferCount)
		{
			OPTICK_EVENT();

			VkCommandPoolCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			m_vCreateInfo.queueFamilyIndex = m_pDevice->GetQueue().m_TransferFamily.value();

			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkCreateCommandPool(m_pDevice->GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vCommandPool));
		}

		const std::vector<std::shared_ptr<VulkanCommandBuffer>> VulkanCommandBufferAllocator::CreateCommandBuffers()
		{
			OPTICK_EVENT();

			VkCommandBufferAllocateInfo m_vAllocateInfo = {};
			m_vAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			m_vAllocateInfo.pNext = VK_NULL_HANDLE;
			m_vAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			m_vAllocateInfo.commandPool = m_vCommandPool;
			m_vAllocateInfo.commandBufferCount = m_BufferCount;

			if (pParentAllocator)
				m_vAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY;

			std::vector<VkCommandBuffer> m_vCommandBuffers(m_BufferCount);
			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkAllocateCommandBuffers(m_pDevice->GetLogicalDevice(), &m_vAllocateInfo, m_vCommandBuffers.data()));

			pCommandBuffers.reserve(m_BufferCount);
			for (const auto m_vCommandBuffer : m_vCommandBuffers)
				pCommandBuffers.emplace_back(std::make_shared<VulkanCommandBuffer>(this, m_vCommandBuffer));

			return pCommandBuffers;
		}

		std::unique_ptr<VulkanCommandBufferAllocator> VulkanCommandBufferAllocator::CreateChildAllocator()
		{
			return std::make_unique<VulkanCommandBufferAllocator>(m_pDevice, this, m_BufferCount);
		}

		void VulkanCommandBufferAllocator::Terminate()
		{
			std::vector<VkCommandBuffer> m_vCommandBuffers(m_BufferCount);
			for (uint32_t i = 0; i < m_BufferCount; i++)
			{
				m_vCommandBuffers[i] = pCommandBuffers[i]->GetVulkanCommandBuffer();
				FlagCommandBufferAsTerminated(i);
			}

			m_pDevice->GetDeviceTable().vkFreeCommandBuffers(m_pDevice->GetLogicalDevice(), m_vCommandPool, m_BufferCount, m_vCommandBuffers.data());
			m_pDevice->GetDeviceTable().vkDestroyCommandPool(m_pDevice->GetLogicalDevice(), m_vCommandPool, nullptr);
		}
	}
}