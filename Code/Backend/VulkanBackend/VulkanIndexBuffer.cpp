// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanIndexBuffer.hpp"
#include "VulkanStaggingBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanIndexBuffer::VulkanIndexBuffer(Device& device, UI64 size, UI64 stride)
			: IndexBuffer(device, size, stride),
			vBuffer(device.StaticCast<VulkanDevice>(), size,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		{
		}

		void VulkanIndexBuffer::Resize(UI64 size, BufferResizeMode mode)
		{
		}

		void VulkanIndexBuffer::CopyFromBuffer(const Buffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset)
		{
			vBuffer.CopyFromBuffer(srcBuffer.StaticCast<VulkanStaggingBuffer>().vBuffer, size, srcOffset, dstOffset);
		}

		void VulkanIndexBuffer::Terminate()
		{
			vBuffer.Terminate();
		}
	}
}