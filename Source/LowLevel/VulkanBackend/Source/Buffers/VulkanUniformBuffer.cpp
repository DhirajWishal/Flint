// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/Buffers/VulkanUniformBuffer.h"
#include "VulkanBackend/VulkanResourceContainer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanUniformBuffer::Initialize(VulkanDevice* pDevice, UI64 size)
		{
			this->pDevice = pDevice;
			CreateBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT, size);
			AllocateBufferMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		
			if (pContainer)
				pContainer->RegisterBuffer(this);
		}

		void VulkanUniformBuffer::Terminate()
		{
			DestroyBuffer();
			FreeBufferMemory();
		}

		Interface::UniformBufferHandle CreateUniformBuffer(const Interface::DeviceHandle& deviceHandle, UI64 size)
		{
			VulkanUniformBuffer* pBuffer = new VulkanUniformBuffer();
			pBuffer->Initialize(static_cast<VulkanDevice*>(HandleToPointer(deviceHandle)), size);

			return PointerToHandle<Interface::UniformBufferHandle>(pBuffer);
		}

		void* MapUniformBufferMempory(const Interface::UniformBufferHandle& handle, UI64 size, UI64 offset)
		{
			return static_cast<VulkanUniformBuffer*>(HandleToPointer(handle))->MapMemory(size, offset);
		}

		void UnmapUniformBufferMemory(const Interface::UniformBufferHandle& handle)
		{
			static_cast<VulkanUniformBuffer*>(HandleToPointer(handle))->UnmapMemory();
		}

		void DestroyUniformBuffer(const Interface::UniformBufferHandle& handle)
		{
			VulkanUniformBuffer* pBuffer = static_cast<VulkanUniformBuffer*>(HandleToPointer(handle));
			pBuffer->Terminate();

			delete pBuffer;
		}
	}
}