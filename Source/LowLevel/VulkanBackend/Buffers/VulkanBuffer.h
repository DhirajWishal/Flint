// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Interface/Buffer.h"
#include "VulkanBackend/VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer : public VulkanDeviceBoundObject {
		public:
			VulkanBuffer() {}
			virtual ~VulkanBuffer() {}

			virtual void Initialize(VulkanDevice* pDevice, UI64 size) {}

			VkBuffer GetBuffer() const { return vBuffer; }

		public:
			void* MapMemory(UI64 size, UI64 offset);
			void UnmapMemory();

		protected:
			void CreateBuffer(VkBufferUsageFlags vUsage, UI64 size);
			void DestroyBuffer();

			void AllocateBufferMemory(VkMemoryPropertyFlags vMemoryProperties);
			void FreeBufferMemory();

		protected:
			VkBuffer vBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vBufferMemory = VK_NULL_HANDLE;
		};
	}
}