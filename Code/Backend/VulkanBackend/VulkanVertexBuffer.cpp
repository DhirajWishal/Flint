// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanVertexBuffer.hpp"
#include "VulkanStaggingBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanVertexBuffer::VulkanVertexBuffer(Device& device, UI64 size, const VertexDescriptor& descriptor)
			: VertexBuffer(device, size, descriptor),
			vBuffer(device.StaticCast<VulkanDevice>(), size,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		{
		}

		void VulkanVertexBuffer::CopyFromBuffer(const StaggingBuffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset)
		{
			vBuffer.CopyFromBuffer(srcBuffer.StaticCast<VulkanStaggingBuffer>().vBuffer, size, srcOffset, dstOffset);
		}

		void VulkanVertexBuffer::Terminate()
		{
			vBuffer.Terminate();
		}
	}
}