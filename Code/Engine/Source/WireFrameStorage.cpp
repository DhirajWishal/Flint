// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "WireFrameStorage.hpp"
#include "Core/Error.hpp"

#include "Device.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <algorithm>

namespace Flint
{
	WireFrame WireFrameStorage::LoadFromAssetFile(const std::filesystem::path& file)
	{
		Assimp::Importer importer = {};
		const aiScene* pScene = importer.ReadFile(file.string(), aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_GenUVCoords);

		// Check if the scene could be loaded.
		if (!pScene)
			FLINT_THROW_RUNTIME_ERROR("Failed to load wire frame form the asset file!");

		UI64 vertexStride = mVertexDescriptor.Stride();

		UI64 vertexBufferSize = 0;
		for (UI32 i = 0; i < pScene->mNumMeshes; i++)
			vertexBufferSize += pScene->mMeshes[i]->mNumVertices * vertexStride;

		// Create stagging buffer and map its memory.
		auto staggingBuffer = pDevice->CreateBuffer(BufferType::STAGGING, vertexBufferSize);
		float* pDataStore = static_cast<float*>(staggingBuffer->MapMemory(vertexBufferSize));

		UI64 indexBufferSize = 0;
		std::vector<std::vector<UI32>> indexArrays;

		// Process the meshes.
		WireFrame::DrawSegment segment = {};
		std::vector<WireFrame::DrawSegment> segments = {};
		for (UI32 i = 0; i < pScene->mNumMeshes; i++)
		{
			segment.mVertexOffset += segment.mVertexCount;
			segment.mIndexOffset += segment.mIndexCount;

			auto pMesh = pScene->mMeshes[i];
			segment.mSegmentName = pMesh->mName.C_Str();
			segment.mVertexCount = pMesh->mNumVertices;

			// Resolve vertex data.
			for (UI32 j = 0; j < pMesh->mNumVertices; j++)
			{
				for (auto& attribute : mVertexDescriptor.mAttributeTypes)
				{
					UI64 copyAmount = static_cast<UI32>(attribute.mDataType) / sizeof(float);
					if (copyAmount == 4) copyAmount = 3;

					switch (attribute.mType)
					{
					case Flint::ShaderAttribueType::POSITION:
						if (pMesh->HasPositions())
							std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::NORMAL:
						if (pMesh->HasNormals())
							std::copy(&pMesh->mNormals[j].x, (&pMesh->mNormals[j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::COLOR_0:
						if (pMesh->HasVertexColors(0))
							std::copy(&pMesh->mColors[0][j].r, (&pMesh->mColors[0][j].r) + copyAmount, pDataStore);
						else
							std::fill(pDataStore, pDataStore + (static_cast<UI32>(attribute.mDataType) / sizeof(float)), 1.0f);
						break;

					case Flint::ShaderAttribueType::COLOR_1:
						if (pMesh->HasVertexColors(1))
							std::copy(&pMesh->mColors[1][j].r, (&pMesh->mColors[1][j].r) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::COLOR_2:
						if (pMesh->HasVertexColors(2))
							std::copy(&pMesh->mColors[2][j].r, (&pMesh->mColors[1][j].r) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::COLOR_3:
						if (pMesh->HasVertexColors(3))
							std::copy(&pMesh->mColors[3][j].r, (&pMesh->mColors[2][j].r) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::TEXTURE_COORDINATES_0:
						if (pMesh->HasTextureCoords(0))
							std::copy(&pMesh->mTextureCoords[0][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::TEXTURE_COORDINATES_1:
						if (pMesh->HasTextureCoords(1))
							std::copy(&pMesh->mTextureCoords[1][j].x, (&pMesh->mTextureCoords[1][j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::TEXTURE_COORDINATES_2:
						if (pMesh->HasTextureCoords(2))
							std::copy(&pMesh->mTextureCoords[2][j].x, (&pMesh->mTextureCoords[2][j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::TEXTURE_COORDINATES_3:
						if (pMesh->HasTextureCoords(3))
							std::copy(&pMesh->mTextureCoords[3][j].x, (&pMesh->mTextureCoords[3][j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::TEXTURE_COORDINATES_4:
						if (pMesh->HasTextureCoords(4))
							std::copy(&pMesh->mTextureCoords[4][j].x, (&pMesh->mTextureCoords[4][j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::TEXTURE_COORDINATES_5:
						if (pMesh->HasTextureCoords(5))
							std::copy(&pMesh->mTextureCoords[5][j].x, (&pMesh->mTextureCoords[5][j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::TEXTURE_COORDINATES_6:
						if (pMesh->HasTextureCoords(6))
							std::copy(&pMesh->mTextureCoords[6][j].x, (&pMesh->mTextureCoords[6][j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::TEXTURE_COORDINATES_7:
						if (pMesh->HasTextureCoords(7))
							std::copy(&pMesh->mTextureCoords[7][j].x, (&pMesh->mTextureCoords[7][j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::UV_COORDINATES:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
						//break;

					case Flint::ShaderAttribueType::TANGENT:
						if (pMesh->HasTangentsAndBitangents())
							std::copy(&pMesh->mTangents[j].x, (&pMesh->mTangents[j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::BITANGENT:
						if (pMesh->HasTangentsAndBitangents())
							std::copy(&pMesh->mBitangents[j].x, (&pMesh->mBitangents[j].x) + copyAmount, pDataStore);
						break;

					case Flint::ShaderAttribueType::BONE_ID:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
						//break;

					case Flint::ShaderAttribueType::BONE_WEIGHT:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
						//break;

					case Flint::ShaderAttribueType::CUSTOM:
						break;

					default:
						FLINT_THROW_RUNTIME_ERROR("Invalid or Undefined vertex attribute type!");
						break;
					}

					pDataStore = pDataStore + (static_cast<UI32>(attribute.mDataType) / sizeof(float));
				}
			}

			// Resolve index data.
			aiFace face = {};
			std::vector<UI32> indexes;
			for (UI32 j = 0; j < pMesh->mNumFaces; j++)
			{
				face = pMesh->mFaces[j];
				for (UI32 k = 0; k < face.mNumIndices; k++)
					INSERT_INTO_VECTOR(indexes, face.mIndices[k]);
			}
			segment.mIndexCount = indexes.size();
			indexBufferSize += segment.mIndexCount * sizeof(UI32);

			// Add the current segment to the segment array.
			INSERT_INTO_VECTOR(segments, segment);

			// Add the indexes to the index array.
			INSERT_INTO_VECTOR(indexArrays, std::move(indexes));
		}
		staggingBuffer->UnmapMemory();
		pDevice->DestroyBuffer(staggingBuffer);

		return WireFrame();
	}

	WireFrame WireFrameStorage::LoadFromCache(const std::filesystem::path& file)
	{
		return WireFrame();
	}
}