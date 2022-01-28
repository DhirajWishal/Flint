// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.GraphicsCore.GeometryStore;

import Flint.GraphicsCore.Buffer;
import Flint.GraphicsCore.Shader;

export namespace Flint
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
	class GeometryStore final : public DeviceBoundObject, public std::enable_shared_from_this<GeometryStore>
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
		GeometryStore(const std::shared_ptr<Device>& pDevice, const std::vector<ShaderAttribute>& vertexAttributes, uint64 indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic);
		~GeometryStore() { if (!bIsTerminated) Terminate(); }

		GeometryStore(const GeometryStore&) = delete;
		GeometryStore& operator=(const GeometryStore&) = delete;

		/**
		 * Set data to the geometry store.
		 *
		 * @param pVertexStagingBuffer The stagging buffer containing all the vertex information.
		 * @param pIndexStagingBuffer The stagging buffer containing all the index information.
		 */
		void SetData(const Buffer* pVertexStagingBuffer, const Buffer* pIndexStagingBuffer);

		/**
		 * Set the vertex and index buffers.
		 * This will move in the provided buffers.
		 *
		 * @param pNewVertexBuffer The vertex buffer pointer.
		 * @param pNewIndexBuffer The index buffer pointer.
		 */
		void SetBuffers(std::shared_ptr<Buffer>&& pNewVertexBuffer, std::shared_ptr<Buffer>&& pNewIndexBuffer);

		/**
		 * Add a geometry to the store.
		 *
		 * @param vertexCount The number of vertexes to add.
		 * @param pVertexData The vertex data pointer.
		 * @param indexCount The number of indexes to add.
		 * @param pIndexData The index data to add.
		 * @return The pair of offsets (vertex offset, index offset) in which the geometry is stored.
		 */
		std::pair<uint64, uint64> AddGeometry(uint64 vertexCount, const void* pVertexData, uint64 indexCount, const void* pIndexData);

		/**
		 * Add geometry to the store using buffers.
		 * This method is preferred over the other adding method.
		 *
		 * @param pVertexStagingBuffer The vertex data stored stagging buffer.
		 * @param pIndexStagingBuffer The index data stored stagging buffer.
		 * @return The pair of offsets (vertex offset, index offset) in which the geometry is stored.
		 */
		std::pair<uint64, uint64> AddGeometry(const Buffer* pVertexStagingBuffer, const Buffer* pIndexStagingBuffer);

		/**
		 * Remove a geometry from the store.
		 *
		 * @param vertexOffset The vertex offset in the buffer.
		 * @param vertexCount The number of vertexes to remove.
		 * @param indexOffset The index offset in the buffer.
		 * @param indexCount The number of indexes to remove.
		 */
		void RemoveGeometry(uint64 vertexOffset, uint64 vertexCount, uint64 indexOffset, uint64 indexCount);

	public:
		/**
		 * Terminate the geometry store.
		 */
		virtual void Terminate() override;

	public:
		/**
		 * Get the vertex buffer from the store.
		 *
		 * @return The buffer pointer.
		 */
		const std::shared_ptr<Buffer> GetVertexBuffer() const { return pVertexBuffer; }

		/**
		 * Get the index buffer from the store.
		 *
		 * @return The buffer pointer.
		 */
		const std::shared_ptr<Buffer> GetIndexBuffer() const { return pIndexBuffer; }

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
		const uint64 GetVertexSize() const { return mVertexSize; }

		/**
		 * Get the vertex count.
		 *
		 * @return The number of vertexes stored.
		 */
		const uint64 GetVertexCount() const { return mVertexCount; }

		/**
		 * Get the size of a single index.
		 *
		 * @return The size in bytes.
		 */
		const uint64 GetIndexSize() const { return mIndexSize; }

		/**
		 * Get the index count.
		 *
		 * @return The number of indexes stored.
		 */
		const uint64 GetIndexCount() const { return mIndexCount; }

		/**
		 * Map the vertex buffer to the local address space.
		 * Before mapping, make sure that the buffer memory profile is BufferMemoryProfile::TransferFriendly.
		 *
		 * @return The memory address.
		 */
		void* MapVertexBuffer() const;

		/**
		 * Map the index buffer to the local address space.
		 * Before mapping, make sure that the buffer memory profile is BufferMemoryProfile::TransferFriendly.
		 *
		 * @return The memory address.
		 */
		void* MapIndexBuffer() const;

		/**
		 * Unmap the vertex buffer.
		 */
		void UnmapVertexBuffer();

		/**
		 * Unmap the index buffer.
		 */
		void UnmapIndexBuffer();

	private:
		std::vector<ShaderAttribute> mVertexAttribtues = {};

		std::shared_ptr<Buffer> pVertexBuffer = nullptr;
		std::shared_ptr<Buffer> pIndexBuffer = nullptr;

		uint64 mVertexSize = 0;
		uint64 mIndexSize = 0;

		uint64 mVertexCount = 0;
		uint64 mIndexCount = 0;

		BufferMemoryProfile mMemoryProfile = BufferMemoryProfile::Automatic;
	};
}

module: private;

import Flint.GraphicsCore.Device;

namespace Flint
{
	GeometryStore::GeometryStore(const std::shared_ptr<Device>& pDevice, const std::vector<ShaderAttribute>& vertexAttributes, uint64 indexSize, const BufferMemoryProfile profile)
		: DeviceBoundObject(pDevice), mVertexAttribtues(vertexAttributes), mIndexSize(indexSize), mMemoryProfile(profile)
	{
		if (mVertexAttribtues.empty())
			throw std::invalid_argument("Vertex attributes should not be empty!");

		if (!indexSize)
			throw std::invalid_argument("Index size should be grater than 0!");

		for (const auto attribute : vertexAttributes)
			mVertexSize += static_cast<uint8>(attribute.mDataType);
	}

	void GeometryStore::SetData(const Buffer* pVertexStagingBuffer, const Buffer* pIndexStagingBuffer)
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

	void GeometryStore::SetBuffers(std::shared_ptr<Buffer>&& pNewVertexBuffer, std::shared_ptr<Buffer>&& pNewIndexBuffer)
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

	std::pair<uint64, uint64> GeometryStore::AddGeometry(uint64 vertexCount, const void* pVertexData, uint64 indexCount, const void* pIndexData)
	{
		std::shared_ptr<Buffer> pVertexStagingBuffer = nullptr;
		std::shared_ptr<Buffer> pIndexStagingBuffer = nullptr;

		// Extend the buffer and add vertex data.
		if (vertexCount)
		{
			uint64 srcSize = mVertexCount * mVertexSize;
			uint64 newSize = vertexCount * mVertexSize;

			// Create new stagging buffer and copy content to it.
			pVertexStagingBuffer = pDevice->CreateBuffer(BufferType::Staging, newSize);
			BYTE* pBytes = static_cast<BYTE*>(pVertexStagingBuffer->MapMemory(newSize));
			std::copy(static_cast<const BYTE*>(pVertexData), static_cast<const BYTE*>(pVertexData) + newSize, pBytes);
			pVertexStagingBuffer->UnmapMemory();
		}

		// Extend the buffer and add index data.
		if (indexCount)
		{
			uint64 srcSize = mIndexCount * mIndexSize;
			uint64 newSize = mIndexSize * indexCount;

			// Create new stagging buffer and copy content to it.
			pIndexStagingBuffer = pDevice->CreateBuffer(BufferType::Staging, newSize);
			BYTE* pBytes = static_cast<BYTE*>(pIndexStagingBuffer->MapMemory(newSize));
			std::copy(static_cast<const BYTE*>(pIndexData), static_cast<const BYTE*>(pIndexData) + newSize, pBytes);
			pIndexStagingBuffer->UnmapMemory();
		}

		const std::pair<uint64, uint64> oldExtent = AddGeometry(pVertexStagingBuffer.get(), pIndexStagingBuffer.get());

		if (pVertexStagingBuffer)
			pVertexStagingBuffer->Terminate();

		if (pIndexStagingBuffer)
			pIndexStagingBuffer->Terminate();

		return oldExtent;
	}

	std::pair<uint64, uint64> GeometryStore::AddGeometry(const Buffer* pVertexStagingBuffer, const Buffer* pIndexStagingBuffer)
	{
		const std::pair<uint64, uint64> oldExtent = std::pair<uint64, uint64>(mVertexCount, mIndexCount);

		// Extend the buffer and add vertex data.
		if (pVertexStagingBuffer)
		{
			uint64 srcSize = mVertexCount * mVertexSize;
			uint64 newSize = pVertexStagingBuffer->GetSize();

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
			uint64 srcSize = mIndexCount * mIndexSize;
			uint64 newSize = pIndexStagingBuffer->GetSize();

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

	void GeometryStore::RemoveGeometry(uint64 vertexOffset, uint64 vertexCount, uint64 indexOffset, uint64 indexCount)
	{
		// Shrink the vertex buffer.
		if (pVertexBuffer)
		{
			std::shared_ptr<Buffer> pStagingBuffer1 = nullptr;
			std::shared_ptr<Buffer> pStagingBuffer2 = nullptr;

			if (vertexOffset)
			{
				pStagingBuffer1 = pDevice->CreateBuffer(BufferType::Staging, vertexOffset * mVertexSize);
				pStagingBuffer1->CopyFromBuffer(pVertexBuffer.get(), vertexOffset * mVertexSize, 0, 0);
			}

			if (vertexOffset + vertexCount < mVertexCount)
			{
				uint64 vertexesToCopy = mVertexCount - (vertexOffset + vertexCount);
				pStagingBuffer2 = pDevice->CreateBuffer(BufferType::Staging, vertexesToCopy * mVertexSize);
				pStagingBuffer2->CopyFromBuffer(pVertexBuffer.get(), vertexesToCopy * mVertexSize, (vertexOffset + vertexCount) * mVertexSize, 0);
			}

			mVertexCount -= vertexCount;

			pVertexBuffer->Terminate();
			pVertexBuffer = pDevice->CreateBuffer(Flint::BufferType::Vertex, mVertexCount * mVertexSize, mMemoryProfile);

			uint64 offset = 0;
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
			std::shared_ptr<Buffer> pStagingBuffer1 = nullptr;
			std::shared_ptr<Buffer> pStagingBuffer2 = nullptr;

			if (indexOffset)
			{
				pStagingBuffer1 = pDevice->CreateBuffer(BufferType::Staging, indexOffset * mIndexSize);
				pStagingBuffer1->CopyFromBuffer(pIndexBuffer.get(), indexOffset * mIndexSize, 0, 0);
			}

			if (indexOffset + indexCount < mIndexCount)
			{
				uint64 indexesToCopy = mIndexCount - (indexOffset + indexCount);
				pStagingBuffer2 = pDevice->CreateBuffer(BufferType::Staging, indexesToCopy * mIndexSize);
				pStagingBuffer2->CopyFromBuffer(pIndexBuffer.get(), indexesToCopy * mIndexSize, (indexOffset + indexCount) * mIndexSize, 0);
			}

			mIndexCount -= indexCount;

			pIndexBuffer->Terminate();
			pIndexBuffer = pDevice->CreateBuffer(Flint::BufferType::Index, mIndexCount * mIndexSize, mMemoryProfile);

			uint64 offset = 0;
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

	void GeometryStore::Terminate()
	{
		if (pVertexBuffer)
			pVertexBuffer->Terminate();

		if (pIndexBuffer)
			pIndexBuffer->Terminate();

		bIsTerminated = true;
	}

	void* GeometryStore::MapVertexBuffer() const
	{
		return pVertexBuffer->MapMemory(pVertexBuffer->GetSize());
	}

	void* GeometryStore::MapIndexBuffer() const
	{
		return pIndexBuffer->MapMemory(pIndexBuffer->GetSize());
	}

	void GeometryStore::UnmapVertexBuffer()
	{
		if (pVertexBuffer)
			pVertexBuffer->UnmapMemory();
	}

	void GeometryStore::UnmapIndexBuffer()
	{
		if (pIndexBuffer)
			pIndexBuffer->UnmapMemory();
	}
}