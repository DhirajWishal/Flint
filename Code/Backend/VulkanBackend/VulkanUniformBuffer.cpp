// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanUniformBuffer.hpp"
#include "VulkanStaggingBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanUniformBuffer::VulkanUniformBuffer(Device& device, UI64 size)
			: UniformBuffer(device, size),
			vBuffer(device.StaticCast<VulkanDevice>(), size,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
		}

		void VulkanUniformBuffer::Resize(UI64 size, BufferResizeMode mode)
		{
		}

		void VulkanUniformBuffer::CopyFromBuffer(const Buffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset)
		{
			vBuffer.CopyFromBuffer(srcBuffer.StaticCast<VulkanStaggingBuffer>().vBuffer, size, srcOffset, dstOffset);
		}

		void* VulkanUniformBuffer::MapMemory(UI64 size, UI64 offset)
		{
			return vBuffer.MapMemory(size, offset);
		}

		void VulkanUniformBuffer::UnmapMemory()
		{
			vBuffer.UnmapMemory();
		}

		void VulkanUniformBuffer::Terminate()
		{
			vBuffer.Terminate();
		}
	}
}