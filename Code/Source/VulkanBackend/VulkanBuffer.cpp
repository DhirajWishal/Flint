// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBuffer.hpp"
#include "VulkanOneTimeCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBuffer::VulkanBuffer(VulkanDevice& device, UI64 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties)
			: vDevice(device), mSize(size), vBufferUsage(usage), vMemoryProperties(memoryProperties)
		{
			CreateBuffer();
			CreateBufferMemory();
		}

		void VulkanBuffer::CopyFromBuffer(const VulkanBuffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset)
		{
			VkBufferCopy vBufferCopy = {};
			vBufferCopy.size = size;
			vBufferCopy.srcOffset = srcOffset;
			vBufferCopy.dstOffset = dstOffset;

			VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);
			vkCmdCopyBuffer(vCommandBuffer, srcBuffer.vBuffer, vBuffer, 1, &vBufferCopy);
		}

		void* VulkanBuffer::MapMemory(UI64 size, UI64 offset)
		{
			if (size + offset > mSize)
				FLINT_THROW_OVERFLOW_ERROR("Submitted size and offset goes beyond the buffer dimensions!");
			else if (size <= 0)
				FLINT_THROW_UNDERFLOW_ERROR("Submitted size is invalid!");

			void* pDataStore = nullptr;
			FLINT_VK_ASSERT(vkMapMemory(vDevice.GetLogicalDevice(), vMemory, offset, size, 0, &pDataStore));

			return pDataStore;
		}

		void VulkanBuffer::UnmapMemory()
		{
			vkUnmapMemory(vDevice.GetLogicalDevice(), vMemory);
		}

		void VulkanBuffer::Terminate()
		{
			vkDestroyBuffer(vDevice.GetLogicalDevice(), vBuffer, nullptr);
			vkFreeMemory(vDevice.GetLogicalDevice(), vMemory, nullptr);
		}

		void VulkanBuffer::CreateBuffer()
		{
			VkBufferCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.queueFamilyIndexCount = 0;
			vCI.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCI.size = static_cast<UI32>(mSize);
			vCI.usage = vBufferUsage;

			FLINT_VK_ASSERT(vkCreateBuffer(vDevice.GetLogicalDevice(), &vCI, nullptr, &vBuffer));
		}

		void VulkanBuffer::CreateBufferMemory()
		{
			VkMemoryRequirements vMR = {};
			vkGetBufferMemoryRequirements(vDevice.GetLogicalDevice(), vBuffer, &vMR);

			VkPhysicalDeviceMemoryProperties vMP = {};
			vkGetPhysicalDeviceMemoryProperties(vDevice.GetPhysicalDevice(), &vMP);

			VkMemoryAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vAI.allocationSize = vMR.size;

			for (UI32 i = 0; i < vMP.memoryTypeCount; i++)
			{
				if ((vMR.memoryTypeBits & (1 << i)) && (vMP.memoryTypes[i].propertyFlags & vMemoryProperties) == vMemoryProperties)
				{
					vAI.memoryTypeIndex = i;
					break;
				}
			}

			FLINT_VK_ASSERT(vkAllocateMemory(vDevice.GetLogicalDevice(), &vAI, nullptr, &vMemory));
			FLINT_VK_ASSERT(vkBindBufferMemory(vDevice.GetLogicalDevice(), vBuffer, vMemory, vMR.size));
		}
	}
}