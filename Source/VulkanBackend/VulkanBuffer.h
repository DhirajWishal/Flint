// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Buffer.h"
#include "VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer final : public FBuffer {
		public:
			VulkanBuffer() {}
			~VulkanBuffer() {}

			virtual void Initialize(FDevice* pDevice, UI64 size, BufferUsage usage, MemoryProfile profile) override final;
			virtual void Terminate() override final;

			virtual void* MapMemory(UI64 size, UI64 offset) override final;
			virtual void UnmapMemory() override final;
			virtual void FlushMemoryMappings() override final;

			virtual void CopyFrom(const FBuffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) override final;

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