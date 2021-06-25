// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer
		{
		public:
			VulkanBuffer(VulkanDevice& device, UI64 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);

			void CopyFromBuffer(const VulkanBuffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset);
			void* MapMemory(UI64 size, UI64 offset = 0);
			void UnmapMemory();
			void Terminate();

			VkBuffer GetBuffer() const { return vBuffer; }
			VkDeviceMemory GetMemory() const { return vMemory; }

		private:
			void CreateBuffer();
			void CreateBufferMemory();

		private:
			VulkanDevice& vDevice;
			UI64 mSize = 0;
			VkBufferUsageFlags vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
			VkMemoryPropertyFlags vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;

			VkBuffer vBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vMemory = VK_NULL_HANDLE;
		};
	}
}