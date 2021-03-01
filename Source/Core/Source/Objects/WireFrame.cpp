// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Objects/WireFrame.h"
#include "Core/ErrorHandler/Logger.h"

#include "Core/Maths/Vector/Vector3.h"

#include <fstream>

namespace Flint
{
	namespace _Helpers
	{
		template<class Type>
		std::ofstream& ToFile(std::ofstream& file, Type value)
		{
			file.write(reinterpret_cast<char*>(&value), sizeof(Type));
			return file;
		}

		template<class Type>
		std::ifstream& FromFile(std::ifstream& file, Type& value)
		{
			file.read(reinterpret_cast<char*>(&value), sizeof(Type));
			return file;
		}
	}

	std::vector<VertexAttribute> CreateDefaultAttributes()
	{
		std::vector<VertexAttribute> attributes(2);

		attributes[0].mType = VertexAttributeType::POSITION;
		attributes[0].mSize = sizeof(Vector3);

		attributes[1].mType = VertexAttributeType::COLOR_0;
		attributes[1].mSize = sizeof(Vector3);

		return attributes;
	}

	WireFrame::WireFrame(const WireFrame& other)
		: pVertexBuffer(other.pVertexBuffer), pIndexBuffer(other.pIndexBuffer), mDrawData(other.mDrawData), mAttributes(other.mAttributes)
	{
	}

	WireFrame::WireFrame(WireFrame&& other) noexcept
		: pVertexBuffer(other.pVertexBuffer), pIndexBuffer(other.pIndexBuffer), mDrawData(std::move(other.mDrawData)), mAttributes(std::move(other.mAttributes))
	{
		other.pVertexBuffer = nullptr;
		other.pIndexBuffer = nullptr;
	}

	void WireFrame::Clear()
	{
		if (pVertexBuffer) pVertexBuffer->Terminate(), delete pVertexBuffer;
		if (pIndexBuffer) pIndexBuffer->Terminate(), delete pIndexBuffer;

		mDrawData.clear();
	}

	void WireFrame::CreateCache(std::filesystem::path title)
	{
		std::ofstream file(title.string() + ".wfc", std::ios::binary);
		if (!file.is_open()) return;

		//The first 1 byte contains the number of vertex attributes.
		file << static_cast<UI8>(mAttributes.size()) << std::endl;

		// Then we return the attributes.
		for (auto& attribute : mAttributes)
			file << attribute.mSize << std::endl << static_cast<UI8>(attribute.mType) << std::endl;

		// Next comes the 8 byte value stating the vertex byte size.
		UI64 vertexBufferSize = pVertexBuffer->GetSize();
		file << vertexBufferSize << std::endl;

		// The vertex data is next.
		{
			// Create stagging buffer and copy vertex data to it.
			Backend::Buffer* pStaggingBuffer = pVertexBuffer->GetDevice()->CreateBuffer(vertexBufferSize, Backend::BufferUsage::STAGGING, Backend::MemoryProfile::TRANSFER_FRIENDLY);
			pStaggingBuffer->CopyFrom(pVertexBuffer, vertexBufferSize, 0, 0);

			// Write and flush vertex data.
			file.write(static_cast<char*>(pStaggingBuffer->MapMemory(vertexBufferSize, 0)), vertexBufferSize);
			pStaggingBuffer->FlushMemoryMappings();
			pStaggingBuffer->UnmapMemory();

			// Terminate the stagging buffer.
			pStaggingBuffer->Terminate();
			delete pStaggingBuffer;
		}

		// Index size and byte size comes next.
		UI64 indexBufferSize = pIndexBuffer->GetSize();
		file << std::endl << indexBufferSize << std::endl;

		// The index data comes next.
		{
			// Create stagging buffer and copy index data to it.
			Backend::Buffer* pStaggingBuffer = pIndexBuffer->GetDevice()->CreateBuffer(indexBufferSize, Backend::BufferUsage::STAGGING, Backend::MemoryProfile::TRANSFER_FRIENDLY);
			pStaggingBuffer->CopyFrom(pIndexBuffer, indexBufferSize, 0, 0);

			// Write and flush index data.
			file.write(static_cast<char*>(pStaggingBuffer->MapMemory(indexBufferSize, 0)), indexBufferSize);
			pStaggingBuffer->FlushMemoryMappings();
			pStaggingBuffer->UnmapMemory();

			// Terminate the stagging buffer.
			pStaggingBuffer->Terminate();
			delete pStaggingBuffer;
		}

		// The next 8 bytes contains the number of draw data.
		file << std::endl << static_cast<UI64>(mDrawData.size()) << std::endl;

		// The next bytes contain the vertex and index count and offset followed by a 4 byte count of a string with the string data.
		for (auto& data : mDrawData)
			file << data.mVertexOffset << std::endl << data.mVertexCount << std::endl << data.mIndexOffset << std::endl << data.mIndexCount << std::endl << data.mName << std::endl;

		// Close the opened file.
		file.close();
	}

	void WireFrame::LoadFromCache(std::filesystem::path asset, Backend::Device* pDevice)
	{
		std::ifstream file(asset, std::ios::binary);
		if (!file.is_open()) return;
		//file.setf(std::ios::hex);

		// The first byte contains the number of vertex attributes.
		UI8 attributeSize = 0;
		file >> attributeSize;
		mAttributes.resize(attributeSize);

		// The next is to load the vertex data.
		for (UI8 i = 0; i < attributeSize; i++)
		{
			VertexAttribute vAttribute = {};
			file >> vAttribute.mSize;

			UI8 type = 0;
			file >> type;
			vAttribute.mType = static_cast<VertexAttributeType>(type);

			mAttributes[i] = std::move(vAttribute);
		}

		// Vertex information.
		{
			// Load the buffer size.
			UI64 byteSize = 0;
			file >> byteSize;
			file.ignore(1);

			// Load the data.
			Backend::Buffer* pStaggingBuffer = pDevice->CreateBuffer(byteSize, Backend::BufferUsage::STAGGING, Backend::MemoryProfile::TRANSFER_FRIENDLY);
			char* pString = static_cast<char*>(pStaggingBuffer->MapMemory(byteSize, 0));
			file.read(static_cast<char*>(pString), byteSize);
			pStaggingBuffer->FlushMemoryMappings();
			pStaggingBuffer->UnmapMemory();

			// Copy data to the buffer.
			pVertexBuffer = pDevice->CreateBuffer(byteSize, Backend::BufferUsage::VERTEX, Backend::MemoryProfile::DRAW_RESOURCE);
			pVertexBuffer->CopyFrom(pStaggingBuffer, byteSize, 0, 0);

			// Destroy stagging buffer.
			pStaggingBuffer->Terminate();
			delete pStaggingBuffer;

			file.ignore(1);
		}

		// Index information.
		{
			// Load the buffer size.
			UI64 byteSize = 0;
			file >> byteSize;
			file.ignore(1);

			// Load the data.
			Backend::Buffer* pStaggingBuffer = pDevice->CreateBuffer(byteSize, Backend::BufferUsage::STAGGING, Backend::MemoryProfile::TRANSFER_FRIENDLY);
			file.read(static_cast<char*>(pStaggingBuffer->MapMemory(byteSize, 0)), byteSize);
			pStaggingBuffer->FlushMemoryMappings();
			pStaggingBuffer->UnmapMemory();

			// Copy data to the buffer.
			pIndexBuffer = pDevice->CreateBuffer(byteSize, Backend::BufferUsage::INDEX, Backend::MemoryProfile::DRAW_RESOURCE);
			pIndexBuffer->CopyFrom(pStaggingBuffer, byteSize, 0, 0);

			// Destroy stagging buffer.
			pStaggingBuffer->Terminate();
			delete pStaggingBuffer;

			file.ignore(1);
		}

		// Get the draw data count.
		UI64 drawCount = 0;
		file >> drawCount;
		mDrawData.resize(drawCount);

		// Load the draw data.
		for (UI32 i = 0; i < drawCount; i++)
		{
			DrawData data = {};
			file >> data.mVertexOffset >> data.mVertexCount >> data.mIndexOffset >> data.mIndexCount;

			file.ignore(1);
			std::getline(file, data.mName);

			mDrawData[i] = std::move(data);
		}

		file.close();
	}

	WireFrame& WireFrame::operator=(const WireFrame& other)
	{
		pVertexBuffer = other.pVertexBuffer;
		pIndexBuffer = other.pIndexBuffer;
		mAttributes = other.mAttributes;
		mDrawData = other.mDrawData;

		return *this;
	}

	WireFrame& WireFrame::operator=(WireFrame&& other) noexcept
	{
		pVertexBuffer = other.pVertexBuffer;
		pIndexBuffer = other.pIndexBuffer;
		mAttributes = std::move(other.mAttributes);
		mDrawData = std::move(other.mDrawData);

		other.pVertexBuffer = nullptr;
		other.pIndexBuffer = nullptr;

		return *this;
	}
}