// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Buffer.h"
#include "VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer : public Backend::Buffer {
		public:
			VulkanBuffer() {}
			~VulkanBuffer() {}

			virtual void Initialize(Backend::Device* pDevice, UI64 size, Backend::BufferUsage usage, Backend::MemoryProfile memoryProfile) override final;
			virtual void Terminate() override final;

		private:
			void CreateBuffer(VkBufferUsageFlags vUsage);
			void DestroyBuffer();

			void AllocateBufferMemory(VkMemoryPropertyFlags vMemoryProperties);
			void FreeBufferMemory();

		private:
			VkBuffer vBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vBufferMemory = VK_NULL_HANDLE;
		};
	}
}