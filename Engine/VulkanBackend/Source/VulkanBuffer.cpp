// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanCommandBuffers.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBuffer::VulkanBuffer(VulkanEngine& engine, uint64_t size, BufferType type)
			: m_Engine(engine), m_Size(size), m_Type(type)
		{
			// Validate the inputs.
			if (m_Size == 0)
				throw BackendError("Invalid buffer size!");

			VmaAllocationCreateFlags vmaFlags = 0;
			auto memoryUsage = VMA_MEMORY_USAGE_UNKNOWN;

			// Setup memory usage.
			switch (m_Type)
			{
			case BufferType::Vertex:
			case BufferType::Index:
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case BufferType::ShallowVertex:
			case BufferType::ShallowIndex:
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				break;

			case BufferType::Storage:
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				break;

			case BufferType::Uniform:
			case BufferType::General:
			case BufferType::Staging:
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
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
			crateInfo.usage = static_cast<VkBufferUsageFlags>(m_Type);
			crateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			crateInfo.queueFamilyIndexCount = 0;
			crateInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
			vmaAllocationCreateInfo.flags = vmaFlags;
			vmaAllocationCreateInfo.usage = memoryUsage;

			FLINT_VK_ASSERT(vmaCreateBuffer(m_Engine.getAllocator(), &crateInfo, &vmaAllocationCreateInfo, &m_Buffer, &m_Allocation, nullptr), "Failed to create the buffer!");

			// Set the descriptor buffer info.
			m_DescriptorBufferInfo.buffer = m_Buffer;
			m_DescriptorBufferInfo.offset = 0;
			m_DescriptorBufferInfo.range = m_Size;
		}

		VulkanBuffer::~VulkanBuffer()
		{
			vmaDestroyBuffer(m_Engine.getAllocator(), m_Buffer, m_Allocation);
		}

		std::byte* VulkanBuffer::mapMemory()
		{
			std::byte* pDataPointer = nullptr;
			FLINT_VK_ASSERT(vmaMapMemory(m_Engine.getAllocator(), m_Allocation, reinterpret_cast<void**>(&pDataPointer)), "Failed to map the buffer memory!");

			m_IsMapped = true;
			return pDataPointer;
		}

		void VulkanBuffer::unmapMemory()
		{
			// We only need to unmap if we have mapped the memory.
			if (m_IsMapped)
			{
				vmaUnmapMemory(m_Engine.getAllocator(), m_Allocation);
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
			auto vCommandBuffer = m_Engine.getUtilityCommandBuffer();
			vCommandBuffer.begin();
			vCommandBuffer.copyBuffer(buffer.m_Buffer, buffer.getSize(), srcOffset, m_Buffer, dstOffset);
			vCommandBuffer.end();
			vCommandBuffer.submitTransfer();
		}
	}
}