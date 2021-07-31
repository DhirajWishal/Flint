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
		const std::pair<UI64, UI64> oldExtent = std::pair<UI64, UI64>(mVertexCount, mIndexCount);

		// Extend the buffer and add vertex data.
		if (vertexCount)
		{
			UI64 srcSize = mVertexCount * mVertexSize;
			UI64 newSize = mVertexSize * vertexCount;

			// Create new stagging buffer and copy content to it.
			std::shared_ptr<Buffer> pStaggingBuffer = pDevice->CreateBuffer(BufferType::STAGGING, newSize);
			BYTE* pBytes = static_cast<BYTE*>(pStaggingBuffer->MapMemory(newSize));
			std::copy(static_cast<const BYTE*>(pVertexData), static_cast<const BYTE*>(pVertexData) + newSize, pBytes);
			pStaggingBuffer->UnmapMemory();

			// Extend and copy data from the stagging buffer.
			if (!pVertexBuffer)
				pVertexBuffer = pDevice->CreateBuffer(BufferType::VERTEX, newSize);
			else
				pVertexBuffer->Extend(newSize, BufferResizeMode::COPY);

			pVertexBuffer->CopyFromBuffer(pStaggingBuffer, newSize, srcSize, 0);
			pDevice->DestroyBuffer(pStaggingBuffer);

			mVertexCount += vertexCount;
		}

		// Extend the buffer and add index data. 
		if (indexCount)
		{
			UI64 srcSize = mIndexCount * mIndexSize;
			UI64 newSize = mIndexSize * indexCount;

			// Create new stagging buffer and copy content to it.
			std::shared_ptr<Buffer> pStaggingBuffer = pDevice->CreateBuffer(BufferType::STAGGING, newSize);
			BYTE* pBytes = static_cast<BYTE*>(pStaggingBuffer->MapMemory(newSize));
			std::copy(static_cast<const BYTE*>(pIndexData), static_cast<const BYTE*>(pIndexData) + newSize, pBytes);
			pStaggingBuffer->UnmapMemory();

			// Extend and copy data from the stagging buffer.
			if (!pIndexBuffer)
				pIndexBuffer = pDevice->CreateBuffer(BufferType::INDEX, newSize);
			else
				pIndexBuffer->Extend(newSize, BufferResizeMode::COPY);

			pIndexBuffer->CopyFromBuffer(pStaggingBuffer, newSize, srcSize, 0);
			pDevice->DestroyBuffer(pStaggingBuffer);

			mIndexCount += indexCount;
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