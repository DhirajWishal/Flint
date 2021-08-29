// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"
#include "GraphicsCore/Buffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer final : public Buffer, public boost::enable_shared_from_this<VulkanBuffer>
		{
		public:
			VulkanBuffer(const boost::shared_ptr<Device>& pDevice, const BufferType type, const UI64 size, const BufferMemoryProfile profile = BufferMemoryProfile::AUTOMATIC);
			~VulkanBuffer() { if (!bIsTerminated) Terminate(); }

			virtual void Resize(const UI64 size, const BufferResizeMode mode) override final;
			virtual void CopyFromBuffer(const boost::shared_ptr<Buffer>& pSrcBuffer, const UI64 size, const UI64 srcOffset, const UI64 dstOffset) override final;
			virtual void* MapMemory(const UI64 size, const UI64 offset = 0) override final;
			virtual void UnmapMemory() override final;
			virtual void Terminate() override final;

			VkBuffer GetBuffer() const { return vBuffer; }
			const VkBuffer* GetBufferAddress() const { return &vBuffer; }
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