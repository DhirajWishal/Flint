// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"
#include "GraphicsCore/Buffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer final : public Buffer, public std::enable_shared_from_this<VulkanBuffer>
		{
		public:
			VulkanBuffer(const std::shared_ptr<Device>& pDevice, const BufferType type, const uint64 size, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic);
			~VulkanBuffer() { if (!bIsTerminated) Terminate(); }

			virtual void Resize(const uint64 size, const BufferResizeMode mode) override;
			virtual void CopyFromBuffer(const Buffer* pSrcBuffer, const uint64 size, const uint64 srcOffset, const uint64 dstOffset) override;
			virtual void* MapMemory(const uint64 size, const uint64 offset = 0) override;
			virtual void UnmapMemory() override;
			virtual void Terminate() override;

			VkBuffer GetBuffer() const { return vBuffer; }
			const VkBuffer* GetBufferAddress() const { return &vBuffer; }

		private:
			void CreateBuffer();

		private:
			VkBuffer vBuffer = VK_NULL_HANDLE;
			VmaAllocation vmaAllocation = {};

			VkBufferUsageFlags vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
			VmaMemoryUsage vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_MAX_ENUM;
		};
	}
}