// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/IndexBuffer.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanStaggingBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanIndexBuffer final : public IndexBuffer
		{
		public:
			VulkanIndexBuffer(Device &device, UI64 size, UI64 stride)
				: IndexBuffer(device, size, stride),
				  vBuffer(device.StaticCast<VulkanDevice>(), size,
						  VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
						  VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			{
			}

			virtual void CopyFromBuffer(const StaggingBuffer &srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) override final
			{
				vBuffer.CopyFromBuffer(srcBuffer.StaticCast<VulkanStaggingBuffer>().vBuffer, size, srcOffset, dstOffset);
			}

			virtual void Terminate() override final
			{
				vBuffer.Terminate();
			}

		private:
			VulkanBuffer vBuffer;
		};
	}
}