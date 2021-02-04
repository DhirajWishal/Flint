// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/Buffers/VulkanStaggingBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanStaggingBuffer::Initialize(VulkanDevice* pDevice, UI64 size)
		{
			this->pDevice = pDevice;

			CreateBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT, size);
			AllocateBufferMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}

		void VulkanStaggingBuffer::Terminate()
		{
			DestroyBuffer();
			FreeBufferMemory();
		}

		Interface::StaggingBufferHandle CreateStaggingBuffer(const Interface::DeviceHandle& deviceHandle, UI64 size)
		{
			VulkanStaggingBuffer* pBuffer = new VulkanStaggingBuffer();
			pBuffer->Initialize(static_cast<VulkanDevice*>(HandleToPointer(deviceHandle)), size);

			return PointerToHandle<Interface::StaggingBufferHandle>(pBuffer);
		}

		void* MapStaggingBufferMempory(const Interface::StaggingBufferHandle& handle, UI64 size, UI64 offset)
		{
			return static_cast<VulkanStaggingBuffer*>(HandleToPointer(handle))->MapMemory(size, offset);
		}

		void UnmapStaggingBufferMemory(const Interface::StaggingBufferHandle& handle)
		{
			static_cast<VulkanStaggingBuffer*>(HandleToPointer(handle))->UnmapMemory();
		}

		void DestroyStaggingBuffer(const Interface::StaggingBufferHandle& handle)
		{
			VulkanStaggingBuffer* pBuffer = static_cast<VulkanStaggingBuffer*>(HandleToPointer(handle));
			pBuffer->Terminate();

			delete pBuffer;
		}
	}
}
