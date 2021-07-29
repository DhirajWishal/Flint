// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBuffer::VulkanBuffer(const std::shared_ptr<Device>& pDevice, BufferType type, const UI64 size)
			: Buffer(pDevice, type, size)
		{
			FLINT_SETUP_PROFILER();

			switch (type)
			{
			case Flint::BufferType::STAGGING:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				break;

			case Flint::BufferType::VERTEX:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				break;

			case Flint::BufferType::INDEX:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				break;

			case Flint::BufferType::UNIFORM:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				break;

			case Flint::BufferType::STORAGE:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				break;

			default:
				FLINT_THROW_RANGE_ERROR("Invalid or Undefined buffer type!");
				break;
			}

			CreateBuffer();
			CreateBufferMemory();
		}

		void VulkanBuffer::Resize(UI64 size, BufferResizeMode mode)
		{
			FLINT_SETUP_PROFILER();
		}

		void VulkanBuffer::CopyFromBuffer(const Buffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset)
		{
			FLINT_SETUP_PROFILER();

			VkBufferCopy vBufferCopy = {};
			vBufferCopy.size = size;
			vBufferCopy.srcOffset = srcOffset;
			vBufferCopy.dstOffset = dstOffset;

			VulkanOneTimeCommandBuffer vCommandBuffer(pDevice->StaticCast<VulkanDevice>());
			vkCmdCopyBuffer(vCommandBuffer, srcBuffer.StaticCast<VulkanBuffer>().vBuffer, vBuffer, 1, &vBufferCopy);
		}

		void* VulkanBuffer::MapMemory(UI64 size, UI64 offset)
		{
			FLINT_SETUP_PROFILER();

			if (size + offset > mSize)
				FLINT_THROW_RANGE_ERROR("Submitted size and offset goes beyond the buffer dimensions!");
			else if (size <= 0)
				FLINT_THROW_RANGE_ERROR("Submitted size is invalid!");

			void* pDataStore = nullptr;
			FLINT_VK_ASSERT(vkMapMemory(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vMemory, offset, size, 0, &pDataStore));

			return pDataStore;
		}

		void VulkanBuffer::UnmapMemory()
		{
			FLINT_SETUP_PROFILER();

			vkUnmapMemory(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vMemory);
		}

		void VulkanBuffer::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vkDestroyBuffer(vDevice.GetLogicalDevice(), vBuffer, nullptr);
			vkFreeMemory(vDevice.GetLogicalDevice(), vMemory, nullptr);
		}

		void VulkanBuffer::CreateBuffer()
		{
			FLINT_SETUP_PROFILER();

			VkBufferCreateInfo vCI = {};
			vCI.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			vCI.flags = 0;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.queueFamilyIndexCount = 0;
			vCI.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCI.size = static_cast<UI32>(mSize);
			vCI.usage = vBufferUsage;

			FLINT_VK_ASSERT(vkCreateBuffer(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCI, nullptr, &vBuffer));
		}

		void VulkanBuffer::CreateBufferMemory()
		{
			FLINT_SETUP_PROFILER();

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			VkMemoryRequirements vMR = {};
			vkGetBufferMemoryRequirements(vDevice.GetLogicalDevice(), vBuffer, &vMR);

			VkPhysicalDeviceMemoryProperties vMP = {};
			vkGetPhysicalDeviceMemoryProperties(vDevice.GetPhysicalDevice(), &vMP);

			VkMemoryAllocateInfo vAI = {};
			vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
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
			FLINT_VK_ASSERT(vkBindBufferMemory(vDevice.GetLogicalDevice(), vBuffer, vMemory, 0));
		}
	}
}