// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Buffer.h"
#include "VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer final : public Backend::Buffer<VulkanBuffer, VulkanDevice> {
		public:
			VulkanBuffer() {}
			~VulkanBuffer() {}

			virtual void Initialize(DeviceType* pDevice, UI64 size, Backend::BufferUsage usage, Backend::MemoryProfile profile) override final;
			virtual void Terminate() override final;

			virtual void* MapMemory(UI64 size, UI64 offset) override final;
			virtual void UnmapMemory() override final;
			virtual void FlushMemoryMappings() override final;

			virtual void CopyFrom(const Derived& buffer, UI64 size, UI64 srcOffset, UI64 dstOffset) override final;

		private:
			void CreateBuffer(VkBufferUsageFlags vUsage);
			void DestroyBuffer();

			void AllocateBufferMemory(VkMemoryPropertyFlags vMemoryProperties);
			void FreeBufferMemory();

		public:
			VkBuffer vBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vBufferMemory = VK_NULL_HANDLE;
		};
	}
}