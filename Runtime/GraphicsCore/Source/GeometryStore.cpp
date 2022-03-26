// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/Device.hpp"

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