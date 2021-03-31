// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Parents/WireFrameManager.h"
#include "Core/ErrorHandler/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define FREEIMAGE_LIB
#include <FreeImage.h>

namespace Flint
{
	WireFrameManager::WireFrameManager()
	{
		FreeImage_Initialise();
	}

	WireFrameManager::~WireFrameManager()
	{
		FreeImage_DeInitialise();
	}

	WireFrame WireFrameManager::CreateNewWireFrame(const char* pAsset, std::vector<VertexAttribute>& vertexAttributes)
	{
		WireFrame wireFrame = {};
		wireFrame.mAttributes = vertexAttributes;

		Assimp::Importer importer = {};
		const aiScene* pScene = importer.ReadFile(pAsset, aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_GenUVCoords);

		// Check if the scene could be loaded.
		if (!pScene)
		{
			FLINT_LOG_ERROR(TEXT("Failed to load wire frame from asset path: #7"), pAsset);
			return {};
		}

		UI64 vertexSize = 0;
		for (auto& attribute : vertexAttributes)
			vertexSize += attribute.mSize;

		UI64 vertexBufferSize = 0;
		for (UI32 i = 0; i < pScene->mNumMeshes; i++)
			vertexBufferSize += pScene->mMeshes[i]->mNumVertices * vertexSize;

		Objects::Buffer staggingVertexBuffer = Objects::CreateBuffer(GetDevice(), vertexBufferSize, Backend::BufferUsage::STAGGING, Backend::MemoryProfile::TRANSFER_FRIENDLY);
		float* pDataStore = static_cast<float*>(staggingVertexBuffer.MapMemory(vertexBufferSize, 0));

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
			INSERT_INTO_VECTOR(wireFrame.mDrawData, drawData);
		}

		staggingVertexBuffer.FlushMemoryMappings();
		staggingVertexBuffer.UnmapMemory();

		wireFrame.mVertexBuffer = Objects::CreateBuffer(GetDevice(), vertexBufferSize, Backend::BufferUsage::VERTEX, Backend::MemoryProfile::DRAW_RESOURCE);
		wireFrame.mVertexBuffer.CopyFrom(staggingVertexBuffer, vertexBufferSize, 0, 0);

		staggingVertexBuffer.Terminate();

		// Create the index buffer.
		{
			Objects::Buffer staggingBuffer = Objects::CreateBuffer(GetDevice(), indexBufferSize, Backend::BufferUsage::STAGGING, Backend::MemoryProfile::TRANSFER_FRIENDLY);
			UI32* pIndexDataStore = static_cast<UI32*>(staggingBuffer.MapMemory(indexBufferSize, 0));

			// Copy data to the stagging buffer.
			UI64 offset = 0;
			for (auto itr = indexArrays.begin(); itr != indexArrays.end(); itr++)
			{
				std::copy(itr->begin(), itr->end(), pIndexDataStore + (offset / sizeof(UI32)));
				offset += sizeof(UI32) * itr->size();
			}
			staggingBuffer.FlushMemoryMappings();
			staggingBuffer.UnmapMemory();

			wireFrame.mIndexBuffer = Objects::CreateBuffer(GetDevice(), indexBufferSize, Backend::BufferUsage::INDEX, Backend::MemoryProfile::DRAW_RESOURCE);
			wireFrame.mIndexBuffer.CopyFrom(staggingBuffer, indexBufferSize, 0, 0);

			staggingBuffer.Terminate();

			indexArrays.clear();
		}

		return std::move(wireFrame);
	}

	Objects::Image WireFrameManager::LoadImageData(std::filesystem::path filePath)
	{
		auto type = FreeImage_GetFileType(filePath.string().c_str());
		auto pImage = FreeImage_Load(type, filePath.string().c_str());

		UI8 bitsPerPixel = static_cast<UI8>(FreeImage_GetBPP(pImage));
		UI32 width = FreeImage_GetWidth(pImage);
		UI32 height = FreeImage_GetHeight(pImage);
		auto channels = FreeImage_GetColorsUsed(pImage);

		Objects::Image image = {};
		image.Initialize(GetDevice(), width, height, 1, Backend::ImageUsage::GRAPHICS_2D, bitsPerPixel);
		image.CopyData(FreeImage_GetBits(pImage), width, 0, height, 0, 1, 0, bitsPerPixel);

		FreeImage_Unload(pImage);
		return image;
	}
}
