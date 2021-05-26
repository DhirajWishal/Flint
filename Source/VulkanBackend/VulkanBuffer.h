// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Buffer.h"
#include "VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer
		{
		public:
			VulkanBuffer() {}

			void Terminate(VulkanDevice* pDevice);
			void* MapMemory(VulkanDevice* pDevice, UI64 size, UI64 offset);
			void UnmapMemory(VulkanDevice* pDevice);

			void CopyFrom(VulkanDevice* pDevice, const Backend::Buffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset);

		protected:
			void CreateBuffer(VulkanDevice* pDevice, UI64 size, VkBufferUsageFlags vUsage);
			void DestroyBuffer(VulkanDevice* pDevice);

			void AllocateBufferMemory(VulkanDevice* pDevice, VkMemoryPropertyFlags vMemoryProperties);
			void FreeBufferMemory(VulkanDevice* pDevice);

		public:
			VkBuffer vBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vBufferMemory = VK_NULL_HANDLE;
		};
	}
}