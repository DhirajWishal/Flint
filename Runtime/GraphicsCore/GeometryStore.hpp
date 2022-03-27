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
			: DeviceBoundObject(pDevice), mVertexAttribtues(vertexAttributes), mIndexSize(indexSize), mMemoryProfile(profile)
		{
			if (mVertexAttribtues.empty())
				throw std::invalid_argument("Vertex attributes should not be empty!");

			if (!indexSize)
				throw std::invalid_argument("Index size should be grater than 0!");

			for (const auto attribute : vertexAttributes)
				mVertexSize += static_cast<uint8_t>(attribute.mDataType);
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
				mVertexCount = pVertexStagingBuffer->GetSize() / mVertexSize;

				if (pVertexBuffer)
					pVertexBuffer->Resize(pVertexStagingBuffer->GetSize(), BufferResizeMode::Clear);
				else
					pVertexBuffer = pDevice->CreateBuffer(BufferType::Vertex, mVertexCount * mVertexSize, mMemoryProfile);

				pVertexBuffer->CopyFromBuffer(pVertexStagingBuffer, pVertexStagingBuffer->GetSize(), 0, 0);
			}

			// Set index information.
			if (pIndexStagingBuffer)
			{
				mIndexCount = pIndexStagingBuffer->GetSize() / mIndexSize;

				if (pIndexBuffer)
					pIndexBuffer->Resize(pIndexStagingBuffer->GetSize(), BufferResizeMode::Clear);
				else
					pIndexBuffer = pDevice->CreateBuffer(BufferType::Index, mIndexCount * mIndexSize, mMemoryProfile);

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
				mVertexCount = pNewVertexBuffer->GetSize() / mVertexSize;
				pVertexBuffer = std::move(pNewVertexBuffer);
			}

			if (pNewIndexBuffer)
			{
				mIndexCount = pNewIndexBuffer->GetSize() / mIndexSize;
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
				uint64_t srcSize = mVertexCount * mVertexSize;
				uint64_t newSize = vertexCount * mVertexSize;

				// Create new stagging buffer and copy content to it.
				pVertexStagingBuffer = pDevice->CreateBuffer(BufferType::Staging, newSize);
				std::byte* pBytes = static_cast<std::byte*>(pVertexStagingBuffer->MapMemory(newSize));
				std::copy(static_cast<const std::byte*>(pVertexData), static_cast<const std::byte*>(pVertexData) + newSize, pBytes);
				pVertexStagingBuffer->UnmapMemory();
			}

			// Extend the buffer and add index data.
			if (indexCount)
			{
				uint64_t srcSize = mIndexCount * mIndexSize;
				uint64_t newSize = mIndexSize * indexCount;

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
			const std::pair<uint64_t, uint64_t> oldExtent = std::pair<uint64_t, uint64_t>(mVertexCount, mIndexCount);

			// Extend the buffer and add vertex data.
			if (pVertexStagingBuffer)
			{
				uint64_t srcSize = mVertexCount * mVertexSize;
				uint64_t newSize = pVertexStagingBuffer->GetSize();

				// Extend and copy data from the stagging buffer.
				if (!pVertexBuffer)
					pVertexBuffer = pDevice->CreateBuffer(BufferType::Vertex, newSize, mMemoryProfile);
				else
					pVertexBuffer->Extend(newSize, BufferResizeMode::Copy);

				pVertexBuffer->CopyFromBuffer(pVertexStagingBuffer, newSize, 0, srcSize);
				mVertexCount += newSize / mVertexSize;
			}

			// Extend the buffer and add index data.
			if (pIndexStagingBuffer)
			{
				uint64_t srcSize = mIndexCount * mIndexSize;
				uint64_t newSize = pIndexStagingBuffer->GetSize();

				// Extend and copy data from the stagging buffer.
				if (!pIndexBuffer)
					pIndexBuffer = pDevice->CreateBuffer(BufferType::Index, newSize, mMemoryProfile);
				else
					pIndexBuffer->Extend(newSize, BufferResizeMode::Copy);

				pIndexBuffer->CopyFromBuffer(pIndexStagingBuffer, newSize, 0, srcSize);
				mIndexCount += newSize / mIndexSize;
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
					pStagingBuffer1 = pDevice->CreateBuffer(BufferType::Staging, vertexOffset * mVertexSize);
					pStagingBuffer1->CopyFromBuffer(pVertexBuffer.get(), vertexOffset * mVertexSize, 0, 0);
				}

				if (vertexOffset + vertexCount < mVertexCount)
				{
					uint64_t vertexesToCopy = mVertexCount - (vertexOffset + vertexCount);
					pStagingBuffer2 = pDevice->CreateBuffer(BufferType::Staging, vertexesToCopy * mVertexSize);
					pStagingBuffer2->CopyFromBuffer(pVertexBuffer.get(), vertexesToCopy * mVertexSize, (vertexOffset + vertexCount) * mVertexSize, 0);
				}

				mVertexCount -= vertexCount;

				pVertexBuffer->Terminate();
				pVertexBuffer = pDevice->CreateBuffer(Flint::BufferType::Vertex, mVertexCount * mVertexSize, mMemoryProfile);

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
					pStagingBuffer1 = pDevice->CreateBuffer(BufferType::Staging, indexOffset * mIndexSize);
					pStagingBuffer1->CopyFromBuffer(pIndexBuffer.get(), indexOffset * mIndexSize, 0, 0);
				}

				if (indexOffset + indexCount < mIndexCount)
				{
					uint64_t indexesToCopy = mIndexCount - (indexOffset + indexCount);
					pStagingBuffer2 = pDevice->CreateBuffer(BufferType::Staging, indexesToCopy * mIndexSize);
					pStagingBuffer2->CopyFromBuffer(pIndexBuffer.get(), indexesToCopy * mIndexSize, (indexOffset + indexCount) * mIndexSize, 0);
				}

				mIndexCount -= indexCount;

				pIndexBuffer->Terminate();
				pIndexBuffer = pDevice->CreateBuffer(Flint::BufferType::Index, mIndexCount * mIndexSize, mMemoryProfile);

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
		const std::vector<ShaderAttribute> GetVertexAttributes() const { return mVertexAttribtues; }

		/**
		 * Get the size of a single vertex.
		 *
		 * @return The size in bytes.
		 */
		const uint64_t GetVertexSize() const { return mVertexSize; }

		/**
		 * Get the vertex count.
		 *
		 * @return The number of vertexes stored.
		 */
		const uint64_t GetVertexCount() const { return mVertexCount; }

		/**
		 * Get the size of a single index.
		 *
		 * @return The size in bytes.
		 */
		const uint64_t GetIndexSize() const { return mIndexSize; }

		/**
		 * Get the index count.
		 *
		 * @return The number of indexes stored.
		 */
		const uint64_t GetIndexCount() const { return mIndexCount; }

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
		std::vector<ShaderAttribute> mVertexAttribtues = {};

		std::unique_ptr<BufferT> pVertexBuffer = nullptr;
		std::unique_ptr<BufferT> pIndexBuffer = nullptr;

		uint64_t mVertexSize = 0;
		uint64_t mIndexSize = 0;

		uint64_t mVertexCount = 0;
		uint64_t mIndexCount = 0;

		BufferMemoryProfile mMemoryProfile = BufferMemoryProfile::Automatic;
	};
}