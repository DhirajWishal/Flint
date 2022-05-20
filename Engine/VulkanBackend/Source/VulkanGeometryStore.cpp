// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanGeometryStore.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanCommandBuffers.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanGeometryStore::VulkanGeometryStore(Engine& engine)
			: GeometryStore(engine)
		{
		}

		VulkanGeometryStore::~VulkanGeometryStore()
		{
			// Destroy the vertex buffer if available.
			if (m_VertexSize)
				destroyBuffer(m_VertexBuffer);

			// Destroy the index buffer if available.
			if (m_IndexSize)
				destroyBuffer(m_IndexBuffer);
		}

		Flint::Geometry VulkanGeometryStore::createGeometry(uint32_t vertexSize, uint32_t indexSize)
		{
			const auto oldVertexSize = m_VertexSize;
			const auto oldIndexSize = m_IndexSize;

			// Extend the buffers.
			extendBuffers(vertexSize, indexSize);

			// Return the geometry.
			return Flint::Geometry(*this, vertexSize, oldVertexSize, indexSize, oldIndexSize);
		}

		std::byte* VulkanGeometryStore::mapVertexData(uint64_t size, uint64_t offset /*= 0*/)
		{
			std::byte* pDataPointer = nullptr;
			FLINT_VK_ASSERT(vmaMapMemory(getEngineAs<VulkanEngine>().getAllocator(), m_VertexBuffer.m_Allocation, reinterpret_cast<void**>(&pDataPointer)), "Failed to map the buffer memory!");

			return pDataPointer + offset;
		}

		void VulkanGeometryStore::unmapVertexData()
		{
			vmaUnmapMemory(getEngineAs<VulkanEngine>().getAllocator(), m_VertexBuffer.m_Allocation);
		}

		std::byte* VulkanGeometryStore::mapIndexData(uint64_t size, uint64_t offset /*= 0*/)
		{
			std::byte* pDataPointer = nullptr;
			FLINT_VK_ASSERT(vmaMapMemory(getEngineAs<VulkanEngine>().getAllocator(), m_IndexBuffer.m_Allocation, reinterpret_cast<void**>(&pDataPointer)), "Failed to map the buffer memory!");

			return pDataPointer + offset;
		}

		void VulkanGeometryStore::unmapIndexData()
		{
			vmaUnmapMemory(getEngineAs<VulkanEngine>().getAllocator(), m_IndexBuffer.m_Allocation);
		}

		void VulkanGeometryStore::extendBuffers(uint32_t vertexSize, uint32_t indexSize)
		{
			// Extend the vertex buffer.
			if (vertexSize > 0)
			{
				// Create the new buffer.
				auto newBuffer = createBuffer(m_VertexSize + vertexSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

				// If there is data in the previous buffer, we can copy it to the new buffer.
				if (m_VertexBuffer.m_Allocation)
				{
					auto pCommandBuffer = getEngineAs<VulkanEngine>().getUtilityCommandBuffer();
					pCommandBuffer.begin();

					// Copy the buffer.
					pCommandBuffer.copyBuffer(m_VertexBuffer.m_Buffer, m_VertexSize, 0, newBuffer.m_Buffer, 0);

					// Submit it.
					pCommandBuffer.end();
					pCommandBuffer.submitTransfer();
					pCommandBuffer.finishExecution();

					// Destroy the previous buffer.
					destroyBuffer(m_VertexBuffer);
				}

				// Finally we can set the new buffer as the main buffer.
				m_VertexBuffer = newBuffer;
				m_VertexSize += vertexSize;
			}

			// Extend the index buffer.
			if (indexSize > 0)
			{
				// Create the new buffer.
				auto newBuffer = createBuffer(m_IndexSize + indexSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

				// If there is data in the previous buffer, we can copy it to the new buffer.
				if (m_IndexBuffer.m_Allocation)
				{
					auto pCommandBuffer = getEngineAs<VulkanEngine>().getUtilityCommandBuffer();
					pCommandBuffer.begin();

					// Copy the buffer.
					pCommandBuffer.copyBuffer(m_IndexBuffer.m_Buffer, m_IndexSize, 0, newBuffer.m_Buffer, 0);

					// Submit it.
					pCommandBuffer.end();
					pCommandBuffer.submitTransfer();
					pCommandBuffer.finishExecution();

					// Destroy the previous buffer.
					destroyBuffer(m_IndexBuffer);
				}

				// Finally we can set the new buffer as the main buffer.
				m_IndexBuffer = newBuffer;
				m_IndexSize += indexSize;
			}
		}

		Flint::VulkanBackend::VulkanGeometryStore::Buffer VulkanGeometryStore::createBuffer(uint64_t size, VkBufferUsageFlags usage) const
		{
			// Setup create info.
			VkBufferCreateInfo crateInfo = {};
			crateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			crateInfo.pNext = nullptr;
			crateInfo.flags = 0;
			crateInfo.size = size;
			crateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;
			crateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			crateInfo.queueFamilyIndexCount = 0;
			crateInfo.pQueueFamilyIndices = nullptr;

			// Setup allocation info.
			VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
			vmaAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			vmaAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

			// Create the buffer.
			Buffer buffer;
			FLINT_VK_ASSERT(vmaCreateBuffer(getEngineAs<VulkanEngine>().getAllocator(), &crateInfo, &vmaAllocationCreateInfo, &buffer.m_Buffer, &buffer.m_Allocation, nullptr), "Failed to create the buffer!");
			return buffer;
		}

		void VulkanGeometryStore::destroyBuffer(Buffer buffer) const
		{
			vmaDestroyBuffer(getEngineAs<VulkanEngine>().getAllocator(), buffer.m_Buffer, buffer.m_Allocation);
		}
	}
}