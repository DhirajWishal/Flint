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

	void GeometryStore::Terminate()
	{
		if (pVertexBuffer)
			pDevice->DestroyBuffer(pVertexBuffer);

		if (pIndexBuffer)
			pDevice->DestroyBuffer(pIndexBuffer);
	}
}