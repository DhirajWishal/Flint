// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanStaggingBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanStaggingBuffer::VulkanStaggingBuffer(Device& device, UI64 size)
			: StaggingBuffer(device, size),
			vBuffer(device.StaticCast<VulkanDevice>(), size,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
		}

		void VulkanStaggingBuffer::Resize(UI64 size, BufferResizeMode mode)
		{
		}

		void VulkanStaggingBuffer::CopyFromBuffer(const Buffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset)
		{
			vBuffer.CopyFromBuffer(srcBuffer.StaticCast<VulkanStaggingBuffer>().vBuffer, size, srcOffset, dstOffset);
		}

		void* VulkanStaggingBuffer::MapMemory(UI64 size, UI64 offset)
		{
			return vBuffer.MapMemory(size, offset);
		}

		void VulkanStaggingBuffer::UnmapMemory()
		{
			vBuffer.UnmapMemory();
		}

		void VulkanStaggingBuffer::Terminate()
		{
			vBuffer.Terminate();
		}
	}
}