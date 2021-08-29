// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBufferAllocator.hpp"
#include "VulkanBackend/VulkanCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBufferAllocator::VulkanCommandBufferAllocator(const boost::shared_ptr<Device>& pDevice, const UI32 bufferCount)
			: CommandBufferAllocator(pDevice, bufferCount)
		{
			FLINT_SETUP_PROFILER();
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			VkCommandPoolCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vCreateInfo.queueFamilyIndex = vDevice.GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vCommandPool));
		}

		VulkanCommandBufferAllocator::VulkanCommandBufferAllocator(const boost::shared_ptr<Device>& pDevice, const boost::shared_ptr<CommandBufferAllocator>& pParent, const UI32 bufferCount)
			: CommandBufferAllocator(pDevice, pParent, bufferCount)
		{
			FLINT_SETUP_PROFILER();
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			VkCommandPoolCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vCreateInfo.queueFamilyIndex = vDevice.GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vCommandPool));
		}

		const boost::container::vector<boost::shared_ptr<CommandBuffer>> VulkanCommandBufferAllocator::CreateCommandBuffers()
		{
			VkCommandBufferAllocateInfo vAllocateInfo = {};
			vAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAllocateInfo.pNext = VK_NULL_HANDLE;
			vAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAllocateInfo.commandPool = vCommandPool;
			vAllocateInfo.commandBufferCount = mBufferCount;

			if (pParentAllocator)
				vAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY;

			boost::container::vector<VkCommandBuffer> vCommandBuffers(mBufferCount);
			FLINT_VK_ASSERT(vkAllocateCommandBuffers(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vAllocateInfo, vCommandBuffers.data()));

			pCommandBuffers.reserve(mBufferCount);
			for (const auto vCommandBuffer : vCommandBuffers)
				pCommandBuffers.push_back(boost::make_shared<VulkanCommandBuffer>(shared_from_this(), vCommandBuffer));

			return pCommandBuffers;
		}

		boost::shared_ptr<CommandBufferAllocator> VulkanCommandBufferAllocator::CreateChildAllocator()
		{
			return boost::make_shared<VulkanCommandBufferAllocator>(pDevice, shared_from_this(), mBufferCount);
		}

		void VulkanCommandBufferAllocator::Terminate()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			boost::container::vector<VkCommandBuffer> vCommandBuffers(mBufferCount);
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