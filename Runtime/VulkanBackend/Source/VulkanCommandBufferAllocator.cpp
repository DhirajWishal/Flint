// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBufferAllocator.hpp"
#include "VulkanBackend/VulkanCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBufferAllocator::VulkanCommandBufferAllocator(VulkanDevice* pDevice, const uint32_t bufferCount)
			: CommandBufferAllocator(pDevice, bufferCount)
		{
			OPTICK_EVENT();

			VkCommandPoolCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vCreateInfo.queueFamilyIndex = pDevice->GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreateCommandPool(pDevice->GetLogicalDevice(), &vCreateInfo, nullptr, &vCommandPool));
		}

		VulkanCommandBufferAllocator::VulkanCommandBufferAllocator(VulkanDevice* pDevice, VulkanCommandBufferAllocator* pParent, const uint32_t bufferCount)
			: CommandBufferAllocator(pDevice, pParent, bufferCount)
		{
			OPTICK_EVENT();

			VkCommandPoolCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vCreateInfo.queueFamilyIndex = pDevice->GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreateCommandPool(pDevice->GetLogicalDevice(), &vCreateInfo, nullptr, &vCommandPool));
		}

		const std::vector<std::shared_ptr<VulkanCommandBuffer>> VulkanCommandBufferAllocator::CreateCommandBuffers()
		{
			OPTICK_EVENT();

			VkCommandBufferAllocateInfo vAllocateInfo = {};
			vAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAllocateInfo.pNext = VK_NULL_HANDLE;
			vAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAllocateInfo.commandPool = vCommandPool;
			vAllocateInfo.commandBufferCount = mBufferCount;

			if (pParentAllocator)
				vAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY;

			std::vector<VkCommandBuffer> vCommandBuffers(mBufferCount);
			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkAllocateCommandBuffers(pDevice->GetLogicalDevice(), &vAllocateInfo, vCommandBuffers.data()));

			pCommandBuffers.reserve(mBufferCount);
			for (const auto vCommandBuffer : vCommandBuffers)
				pCommandBuffers.emplace_back(std::make_shared<VulkanCommandBuffer>(this, vCommandBuffer));

			return pCommandBuffers;
		}

		std::unique_ptr<VulkanCommandBufferAllocator> VulkanCommandBufferAllocator::CreateChildAllocator()
		{
			return std::make_unique<VulkanCommandBufferAllocator>(pDevice, this, mBufferCount);
		}

		void VulkanCommandBufferAllocator::Terminate()
		{
			std::vector<VkCommandBuffer> vCommandBuffers(mBufferCount);
			for (uint32_t i = 0; i < mBufferCount; i++)
			{
				vCommandBuffers[i] = pCommandBuffers[i]->GetVulkanCommandBuffer();
				FlagCommandBufferAsTerminated(i);
			}

			pDevice->GetDeviceTable().vkFreeCommandBuffers(pDevice->GetLogicalDevice(), vCommandPool, mBufferCount, vCommandBuffers.data());
			pDevice->GetDeviceTable().vkDestroyCommandPool(pDevice->GetLogicalDevice(), vCommandPool, nullptr);
		}
	}
}