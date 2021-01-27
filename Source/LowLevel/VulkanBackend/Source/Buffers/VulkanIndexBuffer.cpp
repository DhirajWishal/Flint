// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/Buffers/VulkanIndexBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanIndexBuffer::Initialize(VulkanDevice* pDevice, UI64 size)
		{
			this->pDevice = pDevice;
			CreateBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT, size);
			AllocateBufferMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}

		void VulkanIndexBuffer::Terminate()
		{
			DestroyBuffer();
			FreeBufferMemory();
		}

		Interface::IndexBufferHandle CreateIndexBuffer(const Interface::DeviceHandle& deviceHandle, UI64 size)
		{
			VulkanIndexBuffer* pBuffer = new VulkanIndexBuffer();
			pBuffer->Initialize(static_cast<VulkanDevice*>(HandleToPointer(deviceHandle)), size);

			return PointerToHandle<Interface::IndexBufferHandle>(pBuffer);
		}

		void* MapIndexBufferMempory(const Interface::IndexBufferHandle& handle, UI64 size, UI64 offset)
		{
			return static_cast<VulkanIndexBuffer*>(HandleToPointer(handle))->MapMemory(size, offset);
		}

		void UnmapIndexBufferMemory(const Interface::IndexBufferHandle& handle)
		{
			static_cast<VulkanIndexBuffer*>(HandleToPointer(handle))->UnmapMemory();
		}

		void DestroyIndexBuffer(const Interface::IndexBufferHandle& handle)
		{
			VulkanIndexBuffer* pBuffer = static_cast<VulkanIndexBuffer*>(HandleToPointer(handle));
			pBuffer->Terminate();

			delete pBuffer;
		}
	}
}