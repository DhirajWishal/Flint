// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBuffer.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBuffer::VulkanBuffer(FDevice* pDevice, UI64 size, BufferUsage usage, MemoryProfile profile)
			: FBuffer(pDevice, size, usage, profile)
		{
			VkBufferUsageFlags vBufferUsage = {};
			switch (GetUsage())
			{
			case Flint::BufferUsage::VERTEX:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				break;

			case Flint::BufferUsage::INDEX:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				break;

			case Flint::BufferUsage::UNIFORM:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				break;

			case Flint::BufferUsage::STAGGING:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				break;

			default:
				FLINT_LOG_ERROR(TEXT("Invalid or Undefined buffer usage!"))
					break;
			}

			CreateBuffer(vBufferUsage);

			VkMemoryPropertyFlags vMemoryProperties = {};
			switch (GetMemoryProfile())
			{
			case Flint::MemoryProfile::TRANSFER_FRIENDLY:
				vMemoryProperties =
					VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
					| VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				break;

			case Flint::MemoryProfile::DEVICE_ONLY:
				vMemoryProperties =
					VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					| VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT
					| VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_PROTECTED_BIT;
				break;

			case Flint::MemoryProfile::DRAW_RESOURCE:
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				break;

			default:
				FLINT_LOG_ERROR(TEXT("Invalid or Undefined memory profile!"))
					break;
			}

			AllocateBufferMemory(vMemoryProperties);
		}

		void VulkanBuffer::Terminate()
		{
			DestroyBuffer();
			FreeBufferMemory();
		}

		void* VulkanBuffer::MapMemory(UI64 size, UI64 offset)
		{
			if (size + offset > mSize)
				size = mSize, offset = 0;

			else if (size > mSize)
				size = mSize;

			mPrevMapInfo.mSize = size;
			mPrevMapInfo.mOffset = offset;

			void* pDataStore = nullptr;
			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->MapMemory(vBufferMemory, size, offset, &pDataStore), "Failed to map buffer memory!");
			return pDataStore;
		}

		void VulkanBuffer::UnmapMemory()
		{
			pDevice->GetAs<VulkanDevice>()->UnmapMemory(vBufferMemory);
		}

		void VulkanBuffer::FlushMemoryMappings()
		{
			VkMappedMemoryRange vMMR = {};
			vMMR.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			vMMR.pNext = VK_NULL_HANDLE;
			vMMR.memory = vBufferMemory;
			vMMR.size = mPrevMapInfo.mSize;
			vMMR.offset = mPrevMapInfo.mOffset;

			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->FlushMemoryRanges({ vMMR }), "Failed to flush memory mappings!");
		}

		void VulkanBuffer::CopyFrom(const FBuffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset)
		{
			VkBufferCopy vBC = {};
			vBC.size = size;
			vBC.srcOffset = srcOffset;
			vBC.dstOffset = dstOffset;

			VulkanOneTimeCommandBuffer vCommandBuffer(pDevice->GetAs<VulkanDevice>());
			vkCmdCopyBuffer(vCommandBuffer, pBuffer->GetAs<VulkanBuffer>()->vBuffer, vBuffer, 1, &vBC);
		}

		void VulkanBuffer::CreateBuffer(VkBufferUsageFlags vUsage)
		{
			VkBufferCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.queueFamilyIndexCount = 0;
			vCI.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCI.size = static_cast<UI32>(mSize);
			vCI.usage = vUsage;

			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->CreateBuffer(&vCI, &vBuffer), "Failed to create buffer!");
		}

		void VulkanBuffer::DestroyBuffer()
		{
			pDevice->GetAs<VulkanDevice>()->DestroyBuffer(vBuffer);
		}

		void VulkanBuffer::AllocateBufferMemory(VkMemoryPropertyFlags vMemoryProperties)
		{
			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->CreateBufferMemory({ vBuffer }, vMemoryProperties, &vBufferMemory), "Failed to create buffer memory!");
		}

		void VulkanBuffer::FreeBufferMemory()
		{
			pDevice->GetAs<VulkanDevice>()->FreeMemory(vBufferMemory);
		}
	}
}