// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBufferAllocator.hpp"
#include "VulkanBackend/VulkanCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBufferAllocator::VulkanCommandBufferAllocator(const std::shared_ptr<Device>& pDevice, UI32 bufferCount)
			: CommandBufferAllocator(pDevice, bufferCount)
		{
			FLINT_SETUP_PROFILER();
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			VkCommandPoolCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vCreateInfo.queueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vCommandPool));

			VkCommandBufferAllocateInfo vAllocateInfo = {};
			vAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAllocateInfo.pNext = VK_NULL_HANDLE;
			vAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAllocateInfo.commandPool = vCommandPool;
			vAllocateInfo.commandBufferCount = mBufferCount;

			std::vector<VkCommandBuffer> vCommandBuffers(mBufferCount);
			FLINT_VK_ASSERT(vkAllocateCommandBuffers(vDevice.GetLogicalDevice(), &vAllocateInfo, vCommandBuffers.data()));

			pCommandBuffers.reserve(mBufferCount);
			for (const auto vCommandBuffer : vCommandBuffers)
				pCommandBuffers.push_back(std::make_shared<VulkanCommandBuffer>(shared_from_this(), vCommandBuffer));
		}

		VulkanCommandBufferAllocator::VulkanCommandBufferAllocator(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<CommandBufferAllocator>& pParent, UI32 bufferCount)
			: CommandBufferAllocator(pDevice, pParent, bufferCount)
		{
			FLINT_SETUP_PROFILER();
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			VkCommandPoolCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vCreateInfo.queueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vCommandPool));

			VkCommandBufferAllocateInfo vAllocateInfo = {};
			vAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAllocateInfo.pNext = VK_NULL_HANDLE;
			vAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			vAllocateInfo.commandPool = vCommandPool;
			vAllocateInfo.commandBufferCount = mBufferCount;

			std::vector<VkCommandBuffer> vCommandBuffers(mBufferCount);
			FLINT_VK_ASSERT(vkAllocateCommandBuffers(vDevice.GetLogicalDevice(), &vAllocateInfo, vCommandBuffers.data()));

			pCommandBuffers.reserve(mBufferCount);
			for (const auto vCommandBuffer : vCommandBuffers)
				pCommandBuffers.push_back(std::make_shared<VulkanCommandBuffer>(shared_from_this(), vCommandBuffer));
		}

		std::shared_ptr<CommandBufferAllocator> VulkanCommandBufferAllocator::CreateChildAllocator()
		{
			return std::make_shared<VulkanCommandBufferAllocator>(pDevice, shared_from_this(), mBufferCount);
		}

		void VulkanCommandBufferAllocator::Terminate()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			std::vector<VkCommandBuffer> vCommandBuffers(mBufferCount);
			for (UI32 i = 0; i < mBufferCount; i++)
			{
				vCommandBuffers[i] = pCommandBuffers[i]->StaticCast<VulkanCommandBuffer>().GetVulkanCommandBuffer();
				FlagCommandBufferAsTerminated(i);
			}

			vkFreeCommandBuffers(vDevice.GetLogicalDevice(), vCommandPool, mBufferCount, vCommandBuffers.data());
			vkDestroyCommandPool(vDevice.GetLogicalDevice(), vCommandPool, nullptr);
		}
	}
}