// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/Buffers/VulkanVertexBuffer.h"
#include "VulkanBackend/VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice* pDevice, UI64 size, Backend::MemoryProfile memoryProfile) : VertexBuffer(pDevice, size, memoryProfile)
		{
			CreateBuffer(pDevice, size, VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT);

			VkMemoryPropertyFlags vMemoryProperties = {};
			switch (GetMemoryProfile())
			{
			case Flint::Backend::MemoryProfile::TRANSFER_FRIENDLY:
				vMemoryProperties =
					VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
					| VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				break;

			case Flint::Backend::MemoryProfile::DEVICE_ONLY:
				vMemoryProperties =
					VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					| VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT
					| VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_PROTECTED_BIT;
				break;

			case Flint::Backend::MemoryProfile::DRAW_RESOURCE:
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				break;

			default:
				FLINT_LOG_ERROR(TEXT("Invalid or Undefined memory profile!"))
					break;
			}

			AllocateBufferMemory(pDevice, vMemoryProperties);
		}

		void VulkanVertexBuffer::Terminate()
		{
			VulkanBuffer::Terminate(pDevice->GetAs<VulkanDevice>());
		}

		void* VulkanVertexBuffer::MapMemory(UI64 size, UI64 offset)
		{
			if (size + offset > mSize)
				size = mSize, offset = 0;

			else if (size > mSize)
				size = mSize;

			mPrevMapInfo.mSize = size;
			mPrevMapInfo.mOffset = offset;

			return VulkanBuffer::MapMemory(pDevice->GetAs<VulkanDevice>(), size, offset);
		}

		void VulkanVertexBuffer::UnmapMemory()
		{
			VulkanBuffer::UnmapMemory(pDevice->GetAs<VulkanDevice>());
		}

		void VulkanVertexBuffer::FlushMemoryMappings()
		{
			VkMappedMemoryRange vMMR = {};
			vMMR.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			vMMR.pNext = VK_NULL_HANDLE;
			vMMR.memory = vBufferMemory;
			vMMR.size = mPrevMapInfo.mSize;
			vMMR.offset = mPrevMapInfo.mOffset;

			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->FlushMemoryRanges({ vMMR }), "Failed to flush memory mappings!");
		}

		void VulkanVertexBuffer::CopyFrom(const Backend::Buffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset)
		{
			VulkanBuffer::CopyFrom(pDevice->GetAs<VulkanDevice>(), pBuffer, size, srcOffset, dstOffset);
		}
	}
}