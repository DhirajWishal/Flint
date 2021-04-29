// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Components/WireFrame.h"
#include "Core/ErrorHandler/Logger.h"

#include "Core/Maths/Vector/Vector3.h"

#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

	WireFrame::WireFrame(std::shared_ptr<FDevice> pDevice, std::filesystem::path asset, std::vector<VertexAttribute> vertexAttributes)
	{
		LoadFromFile(pDevice, asset, vertexAttributes);
	}

	WireFrame::WireFrame(std::shared_ptr<FDevice> pDevice, std::filesystem::path asset)
	{
		LoadFromCache(pDevice, asset);
	}

	void WireFrame::Clear()
	{
		pVertexBuffer->Terminate();
		pIndexBuffer->Terminate();

		mDrawData.clear();
	}

	void WireFrame::LoadFromFile(std::shared_ptr<FDevice> pDevice, std::filesystem::path asset, std::vector<VertexAttribute> vertexAttributes)
	{
		Assimp::Importer importer = {};
		const aiScene* pScene = importer.ReadFile(asset.string(), aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_GenUVCoords);

		// Check if the scene could be loaded.
		if (!pScene)
		{
			FLINT_LOG_ERROR(TEXT("Failed to load wire frame from asset path: #8"), asset.string());
			return;
		}

		UI64 vertexSize = 0;
		for (auto& attribute : vertexAttributes)
			vertexSize += attribute.mSize;

		UI64 vertexBufferSize = 0;
		for (UI32 i = 0; i < pScene->mNumMeshes; i++)
			vertexBufferSize += pScene->mMeshes[i]->mNumVertices * vertexSize;

		std::unique_ptr<FBuffer> pStaggingVertexBuffer = pDevice->CreateBufferUnique(vertexBufferSize, BufferUsage::STAGGING, MemoryProfile::TRANSFER_FRIENDLY);
		float* pDataStore = static_cast<float*>(pStaggingVertexBuffer->MapMemory(vertexBufferSize, 0));

		UI64 indexBufferSize = 0;
		std::vector<std::vector<UI32>> indexArrays;

		WireFrame::DrawData drawData = {};
		for (UI32 i = 0; i < pScene->mNumMeshes; i++)
		{
			drawData.mVertexOffset += drawData.mVertexCount;
			drawData.mIndexOffset += drawData.mIndexCount;

			auto pMesh = pScene->mMeshes[i];
			drawData.mName = pMesh->mName.C_Str();
			drawData.mVertexCount = pMesh->mNumVertices;

			for (UI32 j = 0; j < pMesh->mNumVertices; j++)
			{
				for (auto& attribute : vertexAttributes)
				{
					UI64 copyAmount = attribute.mSize / sizeof(float);
					if (copyAmount == 4) copyAmount = 3;

					switch (attribute.mType)
					{
					case Flint::VertexAttributeType::POSITION:
						if (pMesh->HasPositions())
							std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::NORMAL:
						if (pMesh->HasNormals())
							std::copy(&pMesh->mNormals[j].x, (&pMesh->mNormals[j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::COLOR_0:
						if (pMesh->HasVertexColors(0))
							std::copy(&pMesh->mColors[0][j].r, (&pMesh->mColors[0][j].r) + copyAmount, pDataStore);
						else
							std::fill(pDataStore, pDataStore + (attribute.mSize / sizeof(float)), 1.0f);
						break;

					case Flint::VertexAttributeType::COLOR_1:
						if (pMesh->HasVertexColors(1))
							std::copy(&pMesh->mColors[1][j].r, (&pMesh->mColors[1][j].r) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::COLOR_2:
						if (pMesh->HasVertexColors(2))
							std::copy(&pMesh->mColors[2][j].r, (&pMesh->mColors[1][j].r) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::COLOR_3:
						if (pMesh->HasVertexColors(3))
							std::copy(&pMesh->mColors[3][j].r, (&pMesh->mColors[2][j].r) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_0:
						if (pMesh->HasTextureCoords(0))
							std::copy(&pMesh->mTextureCoords[0][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_1:
						if (pMesh->HasTextureCoords(1))
							std::copy(&pMesh->mTextureCoords[1][j].x, (&pMesh->mTextureCoords[1][j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_2:
						if (pMesh->HasTextureCoords(2))
							std::copy(&pMesh->mTextureCoords[2][j].x, (&pMesh->mTextureCoords[2][j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_3:
						if (pMesh->HasTextureCoords(3))
							std::copy(&pMesh->mTextureCoords[3][j].x, (&pMesh->mTextureCoords[3][j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_4:
						if (pMesh->HasTextureCoords(4))
							std::copy(&pMesh->mTextureCoords[4][j].x, (&pMesh->mTextureCoords[4][j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_5:
						if (pMesh->HasTextureCoords(5))
							std::copy(&pMesh->mTextureCoords[5][j].x, (&pMesh->mTextureCoords[5][j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_6:
						if (pMesh->HasTextureCoords(6))
							std::copy(&pMesh->mTextureCoords[6][j].x, (&pMesh->mTextureCoords[6][j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_7:
						if (pMesh->HasTextureCoords(7))
							std::copy(&pMesh->mTextureCoords[7][j].x, (&pMesh->mTextureCoords[7][j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::UV_COORDINATES:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
						//break;

					case Flint::VertexAttributeType::TANGENT:
						if (pMesh->HasTangentsAndBitangents())
							std::copy(&pMesh->mTangents[j].x, (&pMesh->mTangents[j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::BITANGENT:
						if (pMesh->HasTangentsAndBitangents())
							std::copy(&pMesh->mBitangents[j].x, (&pMesh->mBitangents[j].x) + copyAmount, pDataStore);
						break;

					case Flint::VertexAttributeType::BONE_ID:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
						//break;

					case Flint::VertexAttributeType::BONE_WEIGHT:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
						//break;

					case Flint::VertexAttributeType::CUSTOM:
						break;

					default:
						FLINT_LOG_ERROR(TEXT("Invalid or Undefined vertex attribute type!"));
						break;
					}

					pDataStore = pDataStore + (attribute.mSize / sizeof(float));
				}
			}

			aiFace face = {};
			std::vector<UI32> indexes;
			for (UI32 j = 0; j < pMesh->mNumFaces; j++)
			{
				face = pMesh->mFaces[j];
				for (UI32 k = 0; k < face.mNumIndices; k++)
					INSERT_INTO_VECTOR(indexes, face.mIndices[k]);
			}
			drawData.mIndexCount = indexes.size();
			indexBufferSize += drawData.mIndexCount * sizeof(UI32);

			INSERT_INTO_VECTOR(indexArrays, std::move(indexes));
			INSERT_INTO_VECTOR(mDrawData, drawData);
		}

		pStaggingVertexBuffer->FlushMemoryMappings();
		pStaggingVertexBuffer->UnmapMemory();

		pVertexBuffer = pDevice->CreateBufferUnique(vertexBufferSize, BufferUsage::VERTEX, MemoryProfile::DRAW_RESOURCE);
		pVertexBuffer->CopyFrom(pStaggingVertexBuffer.get(), vertexBufferSize, 0, 0);
		pStaggingVertexBuffer->Terminate();

		// Create the index buffer.
		{
			std::unique_ptr<FBuffer> pStaggingBuffer = pDevice->CreateBufferUnique(indexBufferSize, BufferUsage::STAGGING, MemoryProfile::TRANSFER_FRIENDLY);
			UI32* pIndexDataStore = static_cast<UI32*>(pStaggingBuffer->MapMemory(indexBufferSize, 0));

			// Copy data to the stagging buffer.
			UI64 offset = 0;
			for (auto itr = indexArrays.begin(); itr != indexArrays.end(); itr++)
			{
				std::copy(itr->begin(), itr->end(), pIndexDataStore + (offset / sizeof(UI32)));
				offset += sizeof(UI32) * itr->size();
			}
			pStaggingBuffer->FlushMemoryMappings();
			pStaggingBuffer->UnmapMemory();

			pIndexBuffer = pDevice->CreateBufferUnique(indexBufferSize, BufferUsage::INDEX, MemoryProfile::DRAW_RESOURCE);
			pIndexBuffer->CopyFrom(pStaggingBuffer.get(), indexBufferSize, 0, 0);

			pStaggingBuffer->Terminate();
			indexArrays.clear();
		}
	}

	void WireFrame::LoadFromCache(std::shared_ptr<FDevice> pDevice, std::filesystem::path asset)
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
			std::unique_ptr<FBuffer> pStaggingBuffer = pDevice->CreateBufferUnique(byteSize, BufferUsage::STAGGING, MemoryProfile::TRANSFER_FRIENDLY);
			char* pString = static_cast<char*>(pStaggingBuffer->MapMemory(byteSize, 0));
			file.read(static_cast<char*>(pString), byteSize);
			pStaggingBuffer->FlushMemoryMappings();
			pStaggingBuffer->UnmapMemory();

			// Copy data to the buffer.
			pVertexBuffer = pDevice->CreateBufferUnique(byteSize, BufferUsage::VERTEX, MemoryProfile::DRAW_RESOURCE);
			pVertexBuffer->CopyFrom(pStaggingBuffer.get(), byteSize, 0, 0);

			pStaggingBuffer->Terminate();
			file.ignore(1);
		}

		// Index information.
		{
			// Load the buffer size.
			UI64 byteSize = 0;
			file >> byteSize;
			file.ignore(1);

			// Load the data.
			std::unique_ptr<FBuffer> pStaggingBuffer = pDevice->CreateBufferUnique(byteSize, BufferUsage::STAGGING, MemoryProfile::TRANSFER_FRIENDLY);
			file.read(static_cast<char*>(pStaggingBuffer->MapMemory(byteSize, 0)), byteSize);
			pStaggingBuffer->FlushMemoryMappings();
			pStaggingBuffer->UnmapMemory();

			// Copy data to the buffer.
			pIndexBuffer = pDevice->CreateBufferUnique(byteSize, BufferUsage::INDEX, MemoryProfile::DRAW_RESOURCE);
			pIndexBuffer->CopyFrom(pStaggingBuffer.get(), byteSize, 0, 0);

			pStaggingBuffer->Terminate();
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
			std::unique_ptr<FBuffer> pStaggingBuffer = pVertexBuffer->GetDevice()->CreateBufferUnique(vertexBufferSize, BufferUsage::STAGGING, MemoryProfile::TRANSFER_FRIENDLY);
			pStaggingBuffer->CopyFrom(pVertexBuffer.get(), vertexBufferSize, 0, 0);

			// Write and flush vertex data.
			file.write(static_cast<char*>(pStaggingBuffer->MapMemory(vertexBufferSize, 0)), vertexBufferSize);
			pStaggingBuffer->FlushMemoryMappings();
			pStaggingBuffer->UnmapMemory();
			pStaggingBuffer->Terminate();
		}

		// Index size and byte size comes next.
		UI64 indexBufferSize = pVertexBuffer->GetSize();
		file << std::endl << indexBufferSize << std::endl;

		// The index data comes next.
		{
			// Create stagging buffer and copy index data to it.
			std::unique_ptr<FBuffer> pStaggingBuffer = pVertexBuffer->GetDevice()->CreateBufferUnique(indexBufferSize, BufferUsage::STAGGING, MemoryProfile::TRANSFER_FRIENDLY);
			pStaggingBuffer->CopyFrom(pIndexBuffer.get(), indexBufferSize, 0, 0);

			// Write and flush index data.
			file.write(static_cast<char*>(pStaggingBuffer->MapMemory(indexBufferSize, 0)), indexBufferSize);
			pStaggingBuffer->FlushMemoryMappings();
			pStaggingBuffer->UnmapMemory();
			pStaggingBuffer->Terminate();
		}

		// The next 8 bytes contains the number of draw data.
		file << std::endl << static_cast<UI64>(mDrawData.size()) << std::endl;

		// The next bytes contain the vertex and index count and offset followed by a 4 byte count of a string with the string data.
		for (auto& data : mDrawData)
			file << data.mVertexOffset << std::endl << data.mVertexCount << std::endl << data.mIndexOffset << std::endl << data.mIndexCount << std::endl << data.mName << std::endl;

		// Close the opened file.
		file.close();
	}
}