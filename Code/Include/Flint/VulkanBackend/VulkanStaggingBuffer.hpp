// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/StaggingBuffer.hpp"
#include "VulkanBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanStaggingBuffer final : public StaggingBuffer
		{
		public:
			VulkanStaggingBuffer(Device &device, UI64 size)
				: StaggingBuffer(device, size),
				  vBuffer(device.StaticCast<VulkanDevice>(), size,
						  VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
						  VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			{
			}

			virtual void *MapMemory(UI64 size, UI64 offset = 0) override final
			{
				return vBuffer.MapMemory(size, offset);
			}

			virtual void UnmapMemory() override final
			{
				vBuffer.UnmapMemory();
			}

			virtual void Terminate() override final
			{
				vBuffer.Terminate();
			}

		public:
			VulkanBuffer vBuffer;
		};
	}
}