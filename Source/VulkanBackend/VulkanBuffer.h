// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Templates/Buffer.h"
#include "VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer final : public Templates::Buffer
		{
		public:
			VulkanBuffer(VulkanDevice* pDevice, UI64 size, BufferUsage usage, MemoryProfile memoryProfile);

			void Terminate();
			void* MapMemory(UI64 size, UI64 offset);
			void UnmapMemory();

			void CopyFrom(const VulkanBuffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset);

		protected:
			void CreateBuffer(UI64 size, VkBufferUsageFlags vUsage);
			void DestroyBuffer();

			void AllocateBufferMemory(VkMemoryPropertyFlags vMemoryProperties);
			void FreeBufferMemory();

		public:
			VulkanDevice* pDevice = nullptr;
			VkBuffer vBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vBufferMemory = VK_NULL_HANDLE;
		};
	}
}