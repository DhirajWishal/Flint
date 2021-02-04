// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/Buffers/VulkanBuffer.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void* VulkanBuffer::MapMemory(UI64 size, UI64 offset)
		{
			void* pDataStore = nullptr;
			FLINT_VK_ASSERT(pDevice->MapMemory(vBufferMemory, size, offset, &pDataStore), "Failed to map buffer memory!")
			return pDataStore;
		}

		void VulkanBuffer::UnmapMemory()
		{
			pDevice->UnmapMemory(vBufferMemory);
		}

		void VulkanBuffer::CreateBuffer(VkBufferUsageFlags vUsage, UI64 size)
		{
			VkBufferCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.queueFamilyIndexCount = 0;
			vCI.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCI.size = static_cast<UI32>(size);
			vCI.usage = vUsage;

			FLINT_VK_ASSERT(pDevice->CreateBuffer(&vCI, &vBuffer), "Failed to create buffer!")
		}

		void VulkanBuffer::DestroyBuffer()
		{
			pDevice->DestroyBuffer(vBuffer);
		}

		void VulkanBuffer::AllocateBufferMemory(VkMemoryPropertyFlags vMemoryProperties)
		{
			FLINT_VK_ASSERT(pDevice->CreateBufferMemory({ vBuffer }, vMemoryProperties, &vBufferMemory), "Failed to create buffer memory!")
		}

		void VulkanBuffer::FreeBufferMemory()
		{
			pDevice->FreeMemory(vBufferMemory);
		}
	}
}