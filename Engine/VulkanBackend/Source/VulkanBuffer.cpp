// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanCommandBuffers.hpp"

#ifdef FLINT_PLATFORM_WINDOWS
#	include <execution>

#endif

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBuffer::VulkanBuffer(const std::shared_ptr<VulkanDevice>& pDevice, uint64_t size, BufferUsage usage, const std::byte* pDataStore /*= nullptr*/)
			: Buffer(pDevice, size, usage)
		{
			// Create the buffer.
			createBufferAndValidate();

			// Try and copy the data if the user wants us to.
			if (pDataStore)
			{
				// If we can't directly copy, we need to copy it to a staging buffer before we copy it to this.
				if (usage == BufferUsage::Vertex || usage == BufferUsage::Index)
				{
					auto buffer = VulkanBuffer(pDevice, size, BufferUsage::Staging, pDataStore);
					copyFrom(&buffer);
				}
				else
					copyFrom(pDataStore, m_Size, 0, 0);
			}
		}

		VulkanBuffer::~VulkanBuffer()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanBuffer::terminate()
		{
			vmaDestroyBuffer(getDevice().as<VulkanDevice>()->getAllocator(), m_Buffer, m_Allocation);
			invalidate();
		}

		std::byte* VulkanBuffer::mapMemory()
		{
			std::byte* pDataPointer = nullptr;
			FLINT_VK_ASSERT(vmaMapMemory(getDevice().as<VulkanDevice>()->getAllocator(), m_Allocation, reinterpret_cast<void**>(&pDataPointer)), "Failed to map the buffer memory!");

			m_IsMapped = true;
			return pDataPointer;
		}

		void VulkanBuffer::unmapMemory()
		{
			// We only need to unmap if we have mapped the memory.
			if (m_IsMapped)
			{
				vmaUnmapMemory(getDevice().as<VulkanDevice>()->getAllocator(), m_Allocation);
				m_IsMapped = false;
			}
		}

		void VulkanBuffer::copyFrom(const std::byte* pData, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			const auto copySize = size - srcOffset;
			const auto destinationSize = getSize() - dstOffset;

			// Validate the incoming buffer and offsets.
			if (copySize > destinationSize)
				throw BackendError("The data to be copied cannot be stored within this buffer!");

			else if (srcOffset > size)
				throw BackendError("Invalid source offset!");

			else if (dstOffset > m_Size)
				throw BackendError("Invalid destination offset!");

			auto pSource = pData + srcOffset;
			auto pDestination = mapMemory() + dstOffset;

#ifdef FLINT_PLATFORM_WINDOWS
			std::copy_n(std::execution::unseq, pSource, size, pDestination);
#else
			std::copy_n(pSource, size, pDestination);

#endif
			unmapMemory();
		}

		void VulkanBuffer::copyFrom(const Buffer* pBuffer, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			// Setup the command buffer and copy.
			auto vCommandBuffer = VulkanCommandBuffers(getDevicePointerAs<VulkanDevice>());
			vCommandBuffer.begin();

			copyFromBatched(&vCommandBuffer, pBuffer, srcOffset, dstOffset);

			vCommandBuffer.end();
			vCommandBuffer.submitTransfer();
			vCommandBuffer.finishExecution();
		}

		void VulkanBuffer::copyFromBatched(VulkanCommandBuffers* pCommandBuffer, const Buffer* pBuffer, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			const auto copySize = pBuffer->getSize() - srcOffset;
			const auto destinationSize = getSize() - dstOffset;

			// Validate the incoming buffer and offsets.
			if (copySize > destinationSize)
				throw BackendError("The data to be copied cannot be stored within this buffer!");

			else if (srcOffset > pBuffer->getSize())
				throw BackendError("Invalid source offset!");

			else if (dstOffset > m_Size)
				throw BackendError("Invalid destination offset!");

			pCommandBuffer->copyBuffer(pBuffer->as<VulkanBuffer>()->m_Buffer, copySize, srcOffset, m_Buffer, dstOffset);
		}

		void VulkanBuffer::createBufferAndValidate()
		{
			VkBufferUsageFlags bufferUsage = 0;
			VmaAllocationCreateFlags vmaFlags = 0;
			VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_UNKNOWN;

			// Setup usage.
			switch (m_Usage)
			{
			case BufferUsage::Vertex:
				bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case BufferUsage::Index:
				bufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case BufferUsage::ShallowVertex:
				bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case BufferUsage::ShallowIndex:
				bufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case BufferUsage::Uniform:
				bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			case BufferUsage::Storage:
				bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case BufferUsage::General:
				bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			case BufferUsage::Staging:
				bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			default:
				throw BackendError("Invalid buffer type!");
			}

			// Create the buffer.
			VkBufferCreateInfo crateInfo = {};
			crateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			crateInfo.pNext = nullptr;
			crateInfo.flags = 0;
			crateInfo.size = m_Size;
			crateInfo.usage = bufferUsage;
			crateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			crateInfo.queueFamilyIndexCount = 0;
			crateInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
			vmaAllocationCreateInfo.flags = vmaFlags;
			vmaAllocationCreateInfo.usage = memoryUsage;

			FLINT_VK_ASSERT(vmaCreateBuffer(getDevice().as<VulkanDevice>()->getAllocator(), &crateInfo, &vmaAllocationCreateInfo, &m_Buffer, &m_Allocation, nullptr), "Failed to create the buffer!");

			// Set the descriptor buffer info.
			m_DescriptorBufferInfo.buffer = m_Buffer;
			m_DescriptorBufferInfo.offset = 0;
			m_DescriptorBufferInfo.range = m_Size;

			// Make sure to set the object as valid.
			validate();
		}
	}
}