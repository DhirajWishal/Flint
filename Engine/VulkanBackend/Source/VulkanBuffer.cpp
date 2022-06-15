// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanCommandBuffers.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBuffer::VulkanBuffer(VulkanDevice& device, uint64_t size, Core::BufferUsage usage)
			: Buffer(device, size, usage)
		{
			// Validate the inputs.
			if (m_Size == 0)
				throw BackendError("Invalid buffer size!");

			VkBufferUsageFlags bufferUsage = 0;
			VmaAllocationCreateFlags vmaFlags = 0;
			VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_UNKNOWN;

			// Setup usage.
			switch (usage)
			{
			case Core::BufferUsage::Vertex:
				bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case Core::BufferUsage::Index:
				bufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case Core::BufferUsage::ShallowVertex:
				bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case Core::BufferUsage::ShallowIndex:
				bufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case Core::BufferUsage::Uniform:
				bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			case Core::BufferUsage::Storage:
				bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case Core::BufferUsage::General:
				bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			case Core::BufferUsage::Staging:
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

			FLINT_VK_ASSERT(vmaCreateBuffer(getDevice().getAllocator(), &crateInfo, &vmaAllocationCreateInfo, &m_Buffer, &m_Allocation, nullptr), "Failed to create the buffer!");

			// Set the descriptor buffer info.
			m_DescriptorBufferInfo.buffer = m_Buffer;
			m_DescriptorBufferInfo.offset = 0;
			m_DescriptorBufferInfo.range = m_Size;

			// Make sure to set the object as valid.
			validate();
		}

		VulkanBuffer::~VulkanBuffer()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanBuffer::terminate()
		{
			vmaDestroyBuffer(getDevice().getAllocator(), m_Buffer, m_Allocation);
			invalidate();
		}

		std::byte* VulkanBuffer::mapMemory()
		{
			std::byte* pDataPointer = nullptr;
			FLINT_VK_ASSERT(vmaMapMemory(getDevice().getAllocator(), m_Allocation, reinterpret_cast<void**>(&pDataPointer)), "Failed to map the buffer memory!");

			m_IsMapped = true;
			return pDataPointer;
		}

		void VulkanBuffer::unmapMemory()
		{
			// We only need to unmap if we have mapped the memory.
			if (m_IsMapped)
			{
				vmaUnmapMemory(getDevice().getAllocator(), m_Allocation);
				m_IsMapped = false;
			}
		}

		void VulkanBuffer::copyFrom(const VulkanBuffer& buffer, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			// Validate the incoming buffer and offsets.
			if (buffer.getSize() + dstOffset > m_Size)
				throw BackendError("The source buffer size is larger than what's available!");

			else if (m_Size + dstOffset > m_Size)
				throw BackendError("Invalid source offset!");

			else if (buffer.getSize() + dstOffset > buffer.getSize())
				throw BackendError("Invalid destination offset!");


			// Copy the buffer.
			auto vCommandBuffer = getDevice().getUtilityCommandBuffer();
			vCommandBuffer.begin();
			vCommandBuffer.copyBuffer(buffer.m_Buffer, buffer.getSize(), srcOffset, m_Buffer, dstOffset);
			vCommandBuffer.end();
			vCommandBuffer.submitTransfer();
		}
	}
}