// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanStorageBuffer.hpp"
#include "VulkanStaggingBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanStorageBuffer::VulkanStorageBuffer(Device& device, UI64 size)
			: StorageBuffer(device, size),
			vBuffer(device.StaticCast<VulkanDevice>(), size,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
		}

		void VulkanStorageBuffer::Resize(UI64 size, BufferResizeMode mode)
		{
		}

		void VulkanStorageBuffer::CopyFromBuffer(const Buffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset)
		{
			vBuffer.CopyFromBuffer(srcBuffer.StaticCast<VulkanStaggingBuffer>().vBuffer, size, srcOffset, dstOffset);
		}

		void* VulkanStorageBuffer::MapMemory(UI64 size, UI64 offset)
		{
			return vBuffer.MapMemory(size, offset);
		}

		void VulkanStorageBuffer::UnmapMemory()
		{
			vBuffer.UnmapMemory();
		}

		void VulkanStorageBuffer::Terminate()
		{
			vBuffer.Terminate();
		}
	}
}