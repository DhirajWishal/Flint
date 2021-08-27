// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/Device.hpp"

namespace Flint
{
	GeometryStore::GeometryStore(const std::shared_ptr<Device>& pDevice, const std::unordered_map<UI32, std::vector<ShaderAttribute>>& vertexAttributes, UI64 indexSize, const BufferMemoryProfile profile)
		: DeviceBoundObject(pDevice), mVertexAttribtues(vertexAttributes), mIndexSize(indexSize), mMemoryProfile(profile)
	{
		if (mVertexAttribtues.empty())
			FLINT_THROW_INVALID_ARGUMENT("Vertex attributes should not be empty!");

		if (!indexSize)
			FLINT_THROW_INVALID_ARGUMENT("Index size should be grater than 0!");

		for (const auto attributeList : vertexAttributes)
			for (const auto attribute : attributeList.second)
				mVertexSize += static_cast<UI8>(attribute.mDataType);
	}

	void GeometryStore::SetData(const std::shared_ptr<Buffer>& pVertexStagingBuffer, const std::shared_ptr<Buffer>& pIndexStagingBuffer)
	{
		// Set vertex information.
		if (pVertexStagingBuffer)
		{
			mVertexCount = pVertexStagingBuffer->GetSize() / mVertexSize;

			if (pVertexBuffer)
				pVertexBuffer->Resize(pVertexStagingBuffer->GetSize(), BufferResizeMode::CLEAR);
			else
				pVertexBuffer = pDevice->CreateBuffer(BufferType::VERTEX, mVertexCount * mVertexSize, mMemoryProfile);

			pVertexBuffer->CopyFromBuffer(pVertexStagingBuffer, pVertexStagingBuffer->GetSize(), 0, 0);
		}

		// Set index information.
		if (pIndexStagingBuffer)
		{
			mIndexCount = pIndexStagingBuffer->GetSize() / mIndexSize;

			if (pIndexBuffer)
				pIndexBuffer->Resize(pIndexStagingBuffer->GetSize(), BufferResizeMode::CLEAR);
			else
				pIndexBuffer = pDevice->CreateBuffer(BufferType::INDEX, mIndexCount * mIndexSize, mMemoryProfile);

			pIndexBuffer->CopyFromBuffer(pIndexStagingBuffer, pIndexStagingBuffer->GetSize(), 0, 0);
		}
	}

	std::pair<UI64, UI64> GeometryStore::AddGeometry(UI64 vertexCount, const void* pVertexData, UI64 indexCount, const void* pIndexData)
	{
		std::shared_ptr<Buffer> pVertexStagingBuffer = nullptr;
		std::shared_ptr<Buffer> pIndexStagingBuffer = nullptr;

		// Extend the buffer and add vertex data.
		if (vertexCount)
		{
			UI64 srcSize = mVertexCount * mVertexSize;
			UI64 newSize = vertexCount * mVertexSize;

			// Create new stagging buffer and copy content to it.
			pVertexStagingBuffer = pDevice->CreateBuffer(BufferType::STAGING, newSize);
			BYTE* pBytes = static_cast<BYTE*>(pVertexStagingBuffer->MapMemory(newSize));
			std::copy(static_cast<const BYTE*>(pVertexData), static_cast<const BYTE*>(pVertexData) + newSize, pBytes);
			pVertexStagingBuffer->UnmapMemory();
		}

		// Extend the buffer and add index data.
		if (indexCount)
		{
			UI64 srcSize = mIndexCount * mIndexSize;
			UI64 newSize = mIndexSize * indexCount;

			// Create new stagging buffer and copy content to it.
			pIndexStagingBuffer = pDevice->CreateBuffer(BufferType::STAGING, newSize);
			BYTE* pBytes = static_cast<BYTE*>(pIndexStagingBuffer->MapMemory(newSize));
			std::copy(static_cast<const BYTE*>(pIndexData), static_cast<const BYTE*>(pIndexData) + newSize, pBytes);
			pIndexStagingBuffer->UnmapMemory();
		}

		const std::pair<UI64, UI64> oldExtent = AddGeometry(pVertexStagingBuffer, pIndexStagingBuffer);

		if (pVertexStagingBuffer)
			pVertexStagingBuffer->Terminate();

		if (pIndexStagingBuffer)
			pIndexStagingBuffer->Terminate();

		return oldExtent;
	}

	std::pair<UI64, UI64> GeometryStore::AddGeometry(const std::shared_ptr<Buffer>& pVertexStagingBuffer, const std::shared_ptr<Buffer>& pIndexStagingBuffer)
	{
		const std::pair<UI64, UI64> oldExtent = std::pair<UI64, UI64>(mVertexCount, mIndexCount);

		// Extend the buffer and add vertex data.
		if (pVertexStagingBuffer)
		{
			UI64 srcSize = mVertexCount * mVertexSize;
			UI64 newSize = pVertexStagingBuffer->GetSize();

			// Extend and copy data from the stagging buffer.
			if (!pVertexBuffer)
				pVertexBuffer = pDevice->CreateBuffer(BufferType::VERTEX, newSize, mMemoryProfile);
			else
				pVertexBuffer->Extend(newSize, BufferResizeMode::COPY);

			pVertexBuffer->CopyFromBuffer(pVertexStagingBuffer, newSize, 0, srcSize);
			mVertexCount += newSize / mVertexSize;
		}

		// Extend the buffer and add index data.
		if (pIndexStagingBuffer)
		{
			UI64 srcSize = mIndexCount * mIndexSize;
			UI64 newSize = pIndexStagingBuffer->GetSize();

			// Extend and copy data from the stagging buffer.
			if (!pIndexBuffer)
				pIndexBuffer = pDevice->CreateBuffer(BufferType::INDEX, newSize, mMemoryProfile);
			else
				pIndexBuffer->Extend(newSize, BufferResizeMode::COPY);

			pIndexBuffer->CopyFromBuffer(pIndexStagingBuffer, newSize, 0, srcSize);
			mIndexCount += newSize / mIndexSize;
		}

		return oldExtent;
	}

	void GeometryStore::RemoveGeometry(UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount)
	{
		// Shrink the vertex buffer.
		if (pVertexBuffer)
		{
			std::shared_ptr<Buffer> pStagingBuffer1 = nullptr;
			std::shared_ptr<Buffer> pStagingBuffer2 = nullptr;

			if (vertexOffset)
			{
				pStagingBuffer1 = pDevice->CreateBuffer(BufferType::STAGING, vertexOffset * mVertexSize);
				pStagingBuffer1->CopyFromBuffer(pVertexBuffer, vertexOffset * mVertexSize, 0, 0);
			}

			if (vertexOffset + vertexCount < mVertexCount)
			{
				UI64 vertexesToCopy = mVertexCount - (vertexOffset + vertexCount);
				pStagingBuffer2 = pDevice->CreateBuffer(BufferType::STAGING, vertexesToCopy * mVertexSize);
				pStagingBuffer2->CopyFromBuffer(pVertexBuffer, vertexesToCopy * mVertexSize, (vertexOffset + vertexCount) * mVertexSize, 0);
			}

			mVertexCount -= vertexCount;

			pVertexBuffer->Terminate();
			pVertexBuffer = pDevice->CreateBuffer(Flint::BufferType::VERTEX, mVertexCount * mVertexSize, mMemoryProfile);

			UI64 offset = 0;
			if (pStagingBuffer1)
			{
				pVertexBuffer->CopyFromBuffer(pStagingBuffer1, pStagingBuffer1->GetSize(), 0, 0);
				offset += pStagingBuffer1->GetSize();
				pStagingBuffer1->Terminate();
			}

			if (pStagingBuffer2)
			{
				pVertexBuffer->CopyFromBuffer(pStagingBuffer2, pStagingBuffer2->GetSize(), 0, offset);
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
				pStagingBuffer1 = pDevice->CreateBuffer(BufferType::STAGING, indexOffset * mIndexSize);
				pStagingBuffer1->CopyFromBuffer(pIndexBuffer, indexOffset * mIndexSize, 0, 0);
			}

			if (indexOffset + indexCount < mIndexCount)
			{
				UI64 indexesToCopy = mIndexCount - (indexOffset + indexCount);
				pStagingBuffer2 = pDevice->CreateBuffer(BufferType::STAGING, indexesToCopy * mIndexSize);
				pStagingBuffer2->CopyFromBuffer(pIndexBuffer, indexesToCopy * mIndexSize, (indexOffset + indexCount) * mIndexSize, 0);
			}

			mIndexCount -= indexCount;

			pIndexBuffer->Terminate();
			pIndexBuffer = pDevice->CreateBuffer(Flint::BufferType::INDEX, mIndexCount * mIndexSize, mMemoryProfile);

			UI64 offset = 0;
			if (pStagingBuffer1)
			{
				pIndexBuffer->CopyFromBuffer(pStagingBuffer1, pStagingBuffer1->GetSize(), 0, 0);
				offset += pStagingBuffer1->GetSize();
				pStagingBuffer1->Terminate();
			}

			if (pStagingBuffer2)
			{
				pIndexBuffer->CopyFromBuffer(pStagingBuffer2, pStagingBuffer2->GetSize(), 0, offset);
				pStagingBuffer2->Terminate();
			}
		}
	}

	void GeometryStore::Terminate()
	{
		pVertexBuffer->Terminate();
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
		pVertexBuffer->UnmapMemory();
	}

	void GeometryStore::UnmapIndexBuffer()
	{
		pIndexBuffer->UnmapMemory();
	}
}