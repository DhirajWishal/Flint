// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBuffer.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanBuffer::Terminate(VulkanDevice* pDevice)
		{
			DestroyBuffer(pDevice);
			FreeBufferMemory(pDevice);
		}

		void* VulkanBuffer::MapMemory(VulkanDevice* pDevice, UI64 size, UI64 offset)
		{
			void* pDataStore = nullptr;
			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->MapMemory(vBufferMemory, size, offset, &pDataStore), "Failed to map buffer memory!");
			return pDataStore;
		}

		void VulkanBuffer::UnmapMemory(VulkanDevice* pDevice)
		{
			pDevice->GetAs<VulkanDevice>()->UnmapMemory(vBufferMemory);
		}

		void VulkanBuffer::CopyFrom(VulkanDevice* pDevice, const Backend::Buffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset)
		{
			VkBufferCopy vBC = {};
			vBC.size = size;
			vBC.srcOffset = srcOffset;
			vBC.dstOffset = dstOffset;

			VulkanOneTimeCommandBuffer vCommandBuffer(pDevice->GetAs<VulkanDevice>());
			vkCmdCopyBuffer(vCommandBuffer, pBuffer->GetAs<VulkanBuffer>()->vBuffer, vBuffer, 1, &vBC);
		}

		void VulkanBuffer::CreateBuffer(VulkanDevice* pDevice, UI64 size, VkBufferUsageFlags vUsage)
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

			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->CreateBuffer(&vCI, &vBuffer), "Failed to create buffer!");
		}

		void VulkanBuffer::DestroyBuffer(VulkanDevice* pDevice)
		{
			pDevice->GetAs<VulkanDevice>()->DestroyBuffer(vBuffer);
		}

		void VulkanBuffer::AllocateBufferMemory(VulkanDevice* pDevice, VkMemoryPropertyFlags vMemoryProperties)
		{
			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->CreateBufferMemory({ vBuffer }, vMemoryProperties, &vBufferMemory), "Failed to create buffer memory!");
		}

		void VulkanBuffer::FreeBufferMemory(VulkanDevice* pDevice)
		{
			pDevice->GetAs<VulkanDevice>()->FreeMemory(vBufferMemory);
		}
	}
}