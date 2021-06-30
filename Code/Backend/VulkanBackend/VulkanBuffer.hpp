// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"
#include "Buffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer final : public Backend::Buffer
		{
		public:
			VulkanBuffer(const std::shared_ptr<Backend::Device>& pDevice, Backend::BufferType type, const UI64 size);

			virtual void Resize(UI64 size, Backend::BufferResizeMode mode) override final;
			virtual void CopyFromBuffer(const Backend::Buffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) override final;
			virtual void* MapMemory(UI64 size, UI64 offset = 0) override final;
			virtual void UnmapMemory() override final;
			virtual void Terminate() override final;

			VkBuffer GetBuffer() const { return vBuffer; }
			VkDeviceMemory GetMemory() const { return vMemory; }

		private:
			void CreateBuffer();
			void CreateBufferMemory();

		private:
			VkBufferUsageFlags vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
			VkMemoryPropertyFlags vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;

			VkBuffer vBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vMemory = VK_NULL_HANDLE;
		};
	}
}