// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GeometryStore.hpp"
#include "Device.hpp"

namespace Flint
{
	GeometryStore::GeometryStore(const std::shared_ptr<Device>& pDevice, const std::unordered_map<UI32, std::vector<ShaderAttribute>>& vertexAttributes, UI64 indexSize)
		: DeviceBoundObject(pDevice), mVertexAttribtues(vertexAttributes), mIndexSize(indexSize)
	{
		if (mVertexAttribtues.empty())
			FLINT_THROW_INVALID_ARGUMENT("Vertex attributes should not be empty!");

		if (!indexSize)
			FLINT_THROW_INVALID_ARGUMENT("Index size should be grater than 0!");

		for (const auto attributeList : vertexAttributes)
			for (const auto attribute : attributeList.second)
				mVertexSize += static_cast<UI8>(attribute.mDataType);
	}

	std::pair<UI64, UI64> GeometryStore::AddGeometry(UI64 vertexCount, const void* pVertexData, UI64 indexCount, const void* pIndexData)
	{
		std::shared_ptr<Buffer> pVertexStaggingBuffer = nullptr;
		std::shared_ptr<Buffer> pIndexStaggingBuffer = nullptr;

		// Extend the buffer and add vertex data.
		if (vertexCount)
		{
			UI64 srcSize = mVertexCount * mVertexSize;
			UI64 newSize = vertexCount * mVertexSize;

			// Create new stagging buffer and copy content to it.
			pVertexStaggingBuffer = pDevice->CreateBuffer(BufferType::STAGGING, newSize);
			BYTE* pBytes = static_cast<BYTE*>(pVertexStaggingBuffer->MapMemory(newSize));
			std::copy(static_cast<const BYTE*>(pVertexData), static_cast<const BYTE*>(pVertexData) + newSize, pBytes);
			pVertexStaggingBuffer->UnmapMemory();
		}

		// Extend the buffer and add index data. 
		if (indexCount)
		{
			UI64 srcSize = mIndexCount * mIndexSize;
			UI64 newSize = mIndexSize * indexCount;

			// Create new stagging buffer and copy content to it.
			pIndexStaggingBuffer = pDevice->CreateBuffer(BufferType::STAGGING, newSize);
			BYTE* pBytes = static_cast<BYTE*>(pIndexStaggingBuffer->MapMemory(newSize));
			std::copy(static_cast<const BYTE*>(pIndexData), static_cast<const BYTE*>(pIndexData) + newSize, pBytes);
			pIndexStaggingBuffer->UnmapMemory();
		}

		const std::pair<UI64, UI64> oldExtent = AddGeometry(pVertexStaggingBuffer, pIndexStaggingBuffer);

		if (pVertexStaggingBuffer)
			pDevice->DestroyBuffer(pVertexStaggingBuffer);

		if (pIndexStaggingBuffer)
			pDevice->DestroyBuffer(pIndexStaggingBuffer);

		return oldExtent;
	}

	std::pair<UI64, UI64> GeometryStore::AddGeometry(const std::shared_ptr<Buffer>& pVertexStaggingBuffer, const std::shared_ptr<Buffer>& pIndexStaggingBuffer)
	{
		const std::pair<UI64, UI64> oldExtent = std::pair<UI64, UI64>(mVertexCount, mIndexCount);

		// Extend the buffer and add vertex data.
		if (pVertexStaggingBuffer)
		{
			UI64 srcSize = mVertexCount * mVertexSize;
			UI64 newSize = pVertexStaggingBuffer->GetSize();

			// Extend and copy data from the stagging buffer.
			if (!pVertexBuffer)
				pVertexBuffer = pDevice->CreateBuffer(BufferType::VERTEX, newSize);
			else
				pVertexBuffer->Extend(newSize, BufferResizeMode::COPY);

			pVertexBuffer->CopyFromBuffer(pVertexStaggingBuffer, newSize, 0, srcSize);
			mVertexCount += newSize / mVertexSize;
		}

		// Extend the buffer and add index data. 
		if (pIndexStaggingBuffer)
		{
			UI64 srcSize = mIndexCount * mIndexSize;
			UI64 newSize = pIndexStaggingBuffer->GetSize();

			// Extend and copy data from the stagging buffer.
			if (!pIndexBuffer)
				pIndexBuffer = pDevice->CreateBuffer(BufferType::INDEX, newSize);
			else
				pIndexBuffer->Extend(newSize, BufferResizeMode::COPY);

			pIndexBuffer->CopyFromBuffer(pIndexStaggingBuffer, newSize, 0, srcSize);
			mIndexCount += newSize / mIndexSize;
		}

		return oldExtent;
	}

	void GeometryStore::RemoveGeometry(UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount)
	{
		// Shrink the vertex buffer.
		if (pVertexBuffer)
		{
			std::shared_ptr<Buffer> pStaggingBuffer1 = nullptr;
			std::shared_ptr<Buffer> pStaggingBuffer2 = nullptr;

			if (vertexOffset)
			{
				pStaggingBuffer1 = pDevice->CreateBuffer(BufferType::STAGGING, vertexOffset * mVertexSize);
				pStaggingBuffer1->CopyFromBuffer(pVertexBuffer, vertexOffset * mVertexSize, 0, 0);
			}

			if (vertexOffset + vertexCount < mVertexCount)
			{
				UI64 vertexesToCopy = mVertexCount - (vertexOffset + vertexCount);
				pStaggingBuffer2 = pDevice->CreateBuffer(BufferType::STAGGING, vertexesToCopy * mVertexSize);
				pStaggingBuffer2->CopyFromBuffer(pVertexBuffer, vertexesToCopy * mVertexSize, (vertexOffset + vertexCount) * mVertexSize, 0);
			}

			mVertexCount -= vertexCount;

			pDevice->DestroyBuffer(pVertexBuffer);
			pVertexBuffer = pDevice->CreateBuffer(Flint::BufferType::VERTEX, mVertexCount * mVertexSize);

			UI64 offset = 0;
			if (pStaggingBuffer1)
			{
				pVertexBuffer->CopyFromBuffer(pStaggingBuffer1, pStaggingBuffer1->GetSize(), 0, 0);
				offset += pStaggingBuffer1->GetSize();
				pDevice->DestroyBuffer(pStaggingBuffer1);
			}

			if (pStaggingBuffer2)
			{
				pVertexBuffer->CopyFromBuffer(pStaggingBuffer2, pStaggingBuffer2->GetSize(), 0, offset);
				pDevice->DestroyBuffer(pStaggingBuffer2);
			}
		}

		// Shrink the index buffer.
		if (pIndexBuffer)
		{
			std::shared_ptr<Buffer> pStaggingBuffer1 = nullptr;
			std::shared_ptr<Buffer> pStaggingBuffer2 = nullptr;

			if (indexOffset)
			{
				pStaggingBuffer1 = pDevice->CreateBuffer(BufferType::STAGGING, indexOffset * mIndexSize);
				pStaggingBuffer1->CopyFromBuffer(pIndexBuffer, indexOffset * mIndexSize, 0, 0);
			}

			if (indexOffset + indexCount < mIndexCount)
			{
				UI64 indexesToCopy = mIndexCount - (indexOffset + indexCount);
				pStaggingBuffer2 = pDevice->CreateBuffer(BufferType::STAGGING, indexesToCopy * mIndexSize);
				pStaggingBuffer2->CopyFromBuffer(pIndexBuffer, indexesToCopy * mIndexSize, (indexOffset + indexCount) * mIndexSize, 0);
			}

			mIndexCount -= indexCount;

			pDevice->DestroyBuffer(pIndexBuffer);
			pIndexBuffer = pDevice->CreateBuffer(Flint::BufferType::INDEX, mIndexCount * mIndexSize);

			UI64 offset = 0;
			if (pStaggingBuffer1)
			{
				pIndexBuffer->CopyFromBuffer(pStaggingBuffer1, pStaggingBuffer1->GetSize(), 0, 0);
				offset += pStaggingBuffer1->GetSize();
				pDevice->DestroyBuffer(pStaggingBuffer1);
			}

			if (pStaggingBuffer2)
			{
				pIndexBuffer->CopyFromBuffer(pStaggingBuffer2, pStaggingBuffer2->GetSize(), 0, offset);
				pDevice->DestroyBuffer(pStaggingBuffer2);
			}
		}
	}

	void GeometryStore::Terminate()
	{
		if (pVertexBuffer)
			pDevice->DestroyBuffer(pVertexBuffer);

		if (pIndexBuffer)
			pDevice->DestroyBuffer(pIndexBuffer);
	}
}