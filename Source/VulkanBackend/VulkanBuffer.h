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

			virtual void mInitialize() override final;
			virtual void mTerminate() override final;

			virtual void* pMapMemory(UI64 size, UI64 offset) override final;
			virtual void mUnmapMemory() override final;
			virtual void mFlushMemoryMappings() override final;

			virtual void mCopyFrom(Buffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) override final;
			virtual void Bind(const std::shared_ptr<Backend::CommandBuffer>& pCommandBuffer) override final;

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