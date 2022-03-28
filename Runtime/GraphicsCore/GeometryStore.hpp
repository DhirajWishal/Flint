// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"
#include "Shader.hpp"

namespace Flint
{
	/**
	 * Flint geometry store object.
	 * This object is used to store a number of geometry information, which is then passed to render targets and will be rendered using pipelines.
	 * These stores are organized by vertex attributes.
	 *
	 * The reason to use geometry stores is to increase performance. Switching buffers at draw time is costly. Having a large buffer and then drawing everything from it
	 * is comparatively more beneficial than having multiple small buffers. It also helps us to manage device memory better. The reason to organize these stores according
	 * to vertex attributes is that it allows us to store a large group of similar types data.
	 */
	template<class DeviceT, class BufferT>
	class GeometryStore : public DeviceBoundObject<DeviceT>
	{
	public:
		/**
		 * Construct the geometry store.
		 *
		 * @param pDevice The device pointer.
		 * @param vertexAttributes The vertex attributes of the geometry.
		 * @param indexSize The size of a single index in bytes.
		 * @param profile The memory profile of the buffer. Default is BufferMemoryProfile::Automatic.
		 */
		GeometryStore(DeviceT* pDevice, const std::vector<ShaderAttribute>& vertexAttributes, uint64_t indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic)
			: DeviceBoundObject(pDevice), m_VertexAttribtues(vertexAttributes), m_IndexSize(indexSize), m_MemoryProfile(profile)
		{
			if (m_VertexAttribtues.empty())
				throw std::invalid_argument("Vertex attributes should not be empty!");

			if (!indexSize)
				throw std::invalid_argument("Index size should be grater than 0!");

			for (const auto attribute : vertexAttributes)
				m_VertexSize += static_cast<uint8_t>(attribute.m_DataType);
		}

		/**
		 * Default destructor.
		 */
		~GeometryStore() { if (!bIsTerminated) Terminate(); }

		GeometryStore(const GeometryStore&) = delete;
		GeometryStore& operator=(const GeometryStore&) = delete;

		/**
		 * Set data to the geometry store.
		 *
		 * @param pVertexStagingBuffer The stagging buffer containing all the vertex information.
		 * @param pIndexStagingBuffer The stagging buffer containing all the index information.
		 */
		void SetData(const BufferT* pVertexStagingBuffer, const BufferT* pIndexStagingBuffer)
		{
			// Set vertex information.
			if (pVertexStagingBuffer)
			{
				m_VertexCount = pVertexStagingBuffer->GetSize() / m_VertexSize;

				if (pVertexBuffer)
					pVertexBuffer->Resize(pVertexStagingBuffer->GetSize(), BufferResizeMode::Clear);
				else
					pVertexBuffer = pDevice->CreateBuffer(BufferType::Vertex, m_VertexCount * m_VertexSize, m_MemoryProfile);

				pVertexBuffer->CopyFromBuffer(pVertexStagingBuffer, pVertexStagingBuffer->GetSize(), 0, 0);
			}

			// Set index information.
			if (pIndexStagingBuffer)
			{
				m_IndexCount = pIndexStagingBuffer->GetSize() / m_IndexSize;

				if (pIndexBuffer)
					pIndexBuffer->Resize(pIndexStagingBuffer->GetSize(), BufferResizeMode::Clear);
				else
					pIndexBuffer = pDevice->CreateBuffer(BufferType::Index, m_IndexCount * m_IndexSize, m_MemoryProfile);

				pIndexBuffer->CopyFromBuffer(pIndexStagingBuffer, pIndexStagingBuffer->GetSize(), 0, 0);
			}
		}

		/**
		 * Set the vertex and index buffers.
		 * This will move in the provided buffers.
		 * 
		 * @param pNewVertexBuffer The vertex buffer pointer.
		 * @param pNewIndexBuffer The index buffer pointer.
		 */
		void SetBuffers(std::unique_ptr<BufferT>&& pNewVertexBuffer, std::unique_ptr<BufferT>&& pNewIndexBuffer)
		{
			if (pNewVertexBuffer)
			{
				m_VertexCount = pNewVertexBuffer->GetSize() / m_VertexSize;
				pVertexBuffer = std::move(pNewVertexBuffer);
			}

			if (pNewIndexBuffer)
			{
				m_IndexCount = pNewIndexBuffer->GetSize() / m_IndexSize;
				pIndexBuffer = std::move(pNewIndexBuffer);
			}
		}

		/**
		 * Add a geometry to the store.
		 *
		 * @param vertexCount The number of vertexes to add.
		 * @param pVertexData The vertex data pointer.
		 * @param indexCount The number of indexes to add.
		 * @param pIndexData The index data to add.
		 * @return The pair of offsets (vertex offset, index offset) in which the geometry is stored.
		 */
		std::pair<uint64_t, uint64_t> AddGeometry(uint64_t vertexCount, const void* pVertexData, uint64_t indexCount, const void* pIndexData)
		{
			std::unique_ptr<BufferT> pVertexStagingBuffer = nullptr;
			std::unique_ptr<BufferT> pIndexStagingBuffer = nullptr;

			// Extend the buffer and add vertex data.
			if (vertexCount)
			{
				uint64_t srcSize = m_VertexCount * m_VertexSize;
				uint64_t newSize = vertexCount * m_VertexSize;

				// Create new stagging buffer and copy content to it.
				pVertexStagingBuffer = pDevice->CreateBuffer(BufferType::Staging, newSize);
				std::byte* pBytes = static_cast<std::byte*>(pVertexStagingBuffer->MapMemory(newSize));
				std::copy(static_cast<const std::byte*>(pVertexData), static_cast<const std::byte*>(pVertexData) + newSize, pBytes);
				pVertexStagingBuffer->UnmapMemory();
			}

			// Extend the buffer and add index data.
			if (indexCount)
			{
				uint64_t srcSize = m_IndexCount * m_IndexSize;
				uint64_t newSize = m_IndexSize * indexCount;

				// Create new stagging buffer and copy content to it.
				pIndexStagingBuffer = pDevice->CreateBuffer(BufferType::Staging, newSize);
				std::byte* pBytes = static_cast<std::byte*>(pIndexStagingBuffer->MapMemory(newSize));
				std::copy(static_cast<const std::byte*>(pIndexData), static_cast<const std::byte*>(pIndexData) + newSize, pBytes);
				pIndexStagingBuffer->UnmapMemory();
			}

			const std::pair<uint64_t, uint64_t> oldExtent = AddGeometry(pVertexStagingBuffer.get(), pIndexStagingBuffer.get());

			if (pVertexStagingBuffer)
				pVertexStagingBuffer->Terminate();

			if (pIndexStagingBuffer)
				pIndexStagingBuffer->Terminate();

			return oldExtent;
		}

		/**
		 * Add geometry to the store using buffers.
		 * This method is preferred over the other adding method.
		 *
		 * @param pVertexStagingBuffer The vertex data stored stagging buffer.
		 * @param pIndexStagingBuffer The index data stored stagging buffer.
		 * @return The pair of offsets (vertex offset, index offset) in which the geometry is stored.
		 */
		std::pair<uint64_t, uint64_t> AddGeometry(const BufferT* pVertexStagingBuffer, const BufferT* pIndexStagingBuffer)
		{
			const std::pair<uint64_t, uint64_t> oldExtent = std::pair<uint64_t, uint64_t>(m_VertexCount, m_IndexCount);

			// Extend the buffer and add vertex data.
			if (pVertexStagingBuffer)
			{
				uint64_t srcSize = m_VertexCount * m_VertexSize;
				uint64_t newSize = pVertexStagingBuffer->GetSize();

				// Extend and copy data from the stagging buffer.
				if (!pVertexBuffer)
					pVertexBuffer = pDevice->CreateBuffer(BufferType::Vertex, newSize, m_MemoryProfile);
				else
					pVertexBuffer->Extend(newSize, BufferResizeMode::Copy);

				pVertexBuffer->CopyFromBuffer(pVertexStagingBuffer, newSize, 0, srcSize);
				m_VertexCount += newSize / m_VertexSize;
			}

			// Extend the buffer and add index data.
			if (pIndexStagingBuffer)
			{
				uint64_t srcSize = m_IndexCount * m_IndexSize;
				uint64_t newSize = pIndexStagingBuffer->GetSize();

				// Extend and copy data from the stagging buffer.
				if (!pIndexBuffer)
					pIndexBuffer = pDevice->CreateBuffer(BufferType::Index, newSize, m_MemoryProfile);
				else
					pIndexBuffer->Extend(newSize, BufferResizeMode::Copy);

				pIndexBuffer->CopyFromBuffer(pIndexStagingBuffer, newSize, 0, srcSize);
				m_IndexCount += newSize / m_IndexSize;
			}

			return oldExtent;
		}

		/**
		 * Remove a geometry from the store.
		 *
		 * @param vertexOffset The vertex offset in the buffer.
		 * @param vertexCount The number of vertexes to remove.
		 * @param indexOffset The index offset in the buffer.
		 * @param indexCount The number of indexes to remove.
		 */
		void RemoveGeometry(uint64_t vertexOffset, uint64_t vertexCount, uint64_t indexOffset, uint64_t indexCount)
		{
			// Shrink the vertex buffer.
			if (pVertexBuffer)
			{
				std::unique_ptr<BufferT> pStagingBuffer1 = nullptr;
				std::unique_ptr<BufferT> pStagingBuffer2 = nullptr;

				if (vertexOffset)
				{
					pStagingBuffer1 = pDevice->CreateBuffer(BufferType::Staging, vertexOffset * m_VertexSize);
					pStagingBuffer1->CopyFromBuffer(pVertexBuffer.get(), vertexOffset * m_VertexSize, 0, 0);
				}

				if (vertexOffset + vertexCount < m_VertexCount)
				{
					uint64_t vertexesToCopy = m_VertexCount - (vertexOffset + vertexCount);
					pStagingBuffer2 = pDevice->CreateBuffer(BufferType::Staging, vertexesToCopy * m_VertexSize);
					pStagingBuffer2->CopyFromBuffer(pVertexBuffer.get(), vertexesToCopy * m_VertexSize, (vertexOffset + vertexCount) * m_VertexSize, 0);
				}

				m_VertexCount -= vertexCount;

				pVertexBuffer->Terminate();
				pVertexBuffer = pDevice->CreateBuffer(Flint::BufferType::Vertex, m_VertexCount * m_VertexSize, m_MemoryProfile);

				uint64_t offset = 0;
				if (pStagingBuffer1)
				{
					pVertexBuffer->CopyFromBuffer(pStagingBuffer1.get(), pStagingBuffer1->GetSize(), 0, 0);
					offset += pStagingBuffer1->GetSize();
					pStagingBuffer1->Terminate();
				}

				if (pStagingBuffer2)
				{
					pVertexBuffer->CopyFromBuffer(pStagingBuffer2.get(), pStagingBuffer2->GetSize(), 0, offset);
					pStagingBuffer2->Terminate();
				}
			}

			// Shrink the index buffer.
			if (pIndexBuffer)
			{
				std::unique_ptr<BufferT> pStagingBuffer1 = nullptr;
				std::unique_ptr<BufferT> pStagingBuffer2 = nullptr;

				if (indexOffset)
				{
					pStagingBuffer1 = pDevice->CreateBuffer(BufferType::Staging, indexOffset * m_IndexSize);
					pStagingBuffer1->CopyFromBuffer(pIndexBuffer.get(), indexOffset * m_IndexSize, 0, 0);
				}

				if (indexOffset + indexCount < m_IndexCount)
				{
					uint64_t indexesToCopy = m_IndexCount - (indexOffset + indexCount);
					pStagingBuffer2 = pDevice->CreateBuffer(BufferType::Staging, indexesToCopy * m_IndexSize);
					pStagingBuffer2->CopyFromBuffer(pIndexBuffer.get(), indexesToCopy * m_IndexSize, (indexOffset + indexCount) * m_IndexSize, 0);
				}

				m_IndexCount -= indexCount;

				pIndexBuffer->Terminate();
				pIndexBuffer = pDevice->CreateBuffer(Flint::BufferType::Index, m_IndexCount * m_IndexSize, m_MemoryProfile);

				uint64_t offset = 0;
				if (pStagingBuffer1)
				{
					pIndexBuffer->CopyFromBuffer(pStagingBuffer1.get(), pStagingBuffer1->GetSize(), 0, 0);
					offset += pStagingBuffer1->GetSize();
					pStagingBuffer1->Terminate();
				}

				if (pStagingBuffer2)
				{
					pIndexBuffer->CopyFromBuffer(pStagingBuffer2.get(), pStagingBuffer2->GetSize(), 0, offset);
					pStagingBuffer2->Terminate();
				}
			}
		}

	public:
		/**
		 * Terminate the geometry store.
		 */
		virtual void Terminate() override
		{
			if (pVertexBuffer)
				pVertexBuffer->Terminate();

			if (pIndexBuffer)
				pIndexBuffer->Terminate();

			bIsTerminated = true;
		}

	public:
		/**
		 * Get the vertex buffer from the store.
		 *
		 * @return The buffer pointer.
		 */
		BufferT* GetVertexBuffer() const { return pVertexBuffer.get(); }

		/**
		 * Get the index buffer from the store.
		 *
		 * @return The buffer pointer.
		 */
		BufferT* GetIndexBuffer() const { return pIndexBuffer.get(); }

		/**
		 * Get the vertex attributes.
		 *
		 * @return The attribute map.
		 */
		const std::vector<ShaderAttribute> GetVertexAttributes() const { return m_VertexAttribtues; }

		/**
		 * Get the size of a single vertex.
		 *
		 * @return The size in bytes.
		 */
		const uint64_t GetVertexSize() const { return m_VertexSize; }

		/**
		 * Get the vertex count.
		 *
		 * @return The number of vertexes stored.
		 */
		const uint64_t GetVertexCount() const { return m_VertexCount; }

		/**
		 * Get the size of a single index.
		 *
		 * @return The size in bytes.
		 */
		const uint64_t GetIndexSize() const { return m_IndexSize; }

		/**
		 * Get the index count.
		 *
		 * @return The number of indexes stored.
		 */
		const uint64_t GetIndexCount() const { return m_IndexCount; }

		/**
		 * Map the vertex buffer to the local address space.
		 * Before mapping, make sure that the buffer memory profile is BufferMemoryProfile::TransferFriendly.
		 *
		 * @return The memory address.
		 */
		void* MapVertexBuffer() const
		{
			return pVertexBuffer->MapMemory(pVertexBuffer->GetSize());
		}

		/**
		 * Map the index buffer to the local address space.
		 * Before mapping, make sure that the buffer memory profile is BufferMemoryProfile::TransferFriendly.
		 *
		 * @return The memory address.
		 */
		void* MapIndexBuffer() const
		{
			return pIndexBuffer->MapMemory(pIndexBuffer->GetSize());
		}

		/**
		 * Unmap the vertex buffer.
		 */
		void UnmapVertexBuffer()
		{
			if (pVertexBuffer)
				pVertexBuffer->UnmapMemory();
		}

		/**
		 * Unmap the index buffer.
		 */
		void UnmapIndexBuffer()
		{
			if (pIndexBuffer)
				pIndexBuffer->UnmapMemory();
		}

	private:
		std::vector<ShaderAttribute> m_VertexAttribtues = {};

		std::unique_ptr<BufferT> pVertexBuffer = nullptr;
		std::unique_ptr<BufferT> pIndexBuffer = nullptr;

		uint64_t m_VertexSize = 0;
		uint64_t m_IndexSize = 0;

		uint64_t m_VertexCount = 0;
		uint64_t m_IndexCount = 0;

		BufferMemoryProfile m_MemoryProfile = BufferMemoryProfile::Automatic;
	};
}