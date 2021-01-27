// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/Buffers/VulkanVertexBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanVertexBuffer::Initialize(VulkanDevice* pDevice, UI64 size)
		{
			this->pDevice = pDevice;
			CreateBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT, size);
			AllocateBufferMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}

		void VulkanVertexBuffer::Terminate()
		{
			DestroyBuffer();
			FreeBufferMemory();
		}

		Interface::VertexBufferHandle CreateVertexBuffer(const Interface::DeviceHandle& deviceHandle, UI64 size)
		{
			VulkanVertexBuffer* pBuffer = new VulkanVertexBuffer();
			pBuffer->Initialize(static_cast<VulkanDevice*>(HandleToPointer(deviceHandle)), size);

			return PointerToHandle<Interface::VertexBufferHandle>(pBuffer);
		}

		void* MapVertexBufferMempory(const Interface::VertexBufferHandle& handle, UI64 size, UI64 offset)
		{
			return static_cast<VulkanVertexBuffer*>(HandleToPointer(handle))->MapMemory(size, offset);
		}

		void UnmapVertexBufferMemory(const Interface::VertexBufferHandle& handle)
		{
			static_cast<VulkanVertexBuffer*>(HandleToPointer(handle))->UnmapMemory();
		}

		void DestroyVertexBuffer(const Interface::VertexBufferHandle& handle)
		{
			VulkanVertexBuffer* pBuffer = static_cast<VulkanVertexBuffer*>(HandleToPointer(handle));
			pBuffer->Terminate();

			delete pBuffer;
		}
	}
}