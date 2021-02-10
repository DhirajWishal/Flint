// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Parents/WireFrameManager.h"
#include "Core/ErrorHandler/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void* IncrementPointer(void* pPointer, UI64 value)
{
	return reinterpret_cast<void*>(reinterpret_cast<UI64>(pPointer) + value);
}

namespace Flint
{
	WireFrame WireFrameManager::CreateNewWireFrame(const char* pAsset, std::vector<VertexAttribute>& vertexAttributes)
	{
		Assimp::Importer importer = {};
		const aiScene* pScene = importer.ReadFile(pAsset,
			aiProcess_MakeLeftHanded |
			//aiProcess_FlipWindingOrder |
			aiProcess_FlipUVs |
			aiProcess_PreTransformVertices |
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_Triangulate |
			aiProcess_FixInfacingNormals |
			aiProcess_FindInvalidData |
			aiProcess_ValidateDataStructure | 0);

		// Check if the scene could be loaded.
		if (!pScene)
		{
			FLINT_LOG_ERROR(TEXT("Failed to load wire frame from asset path: #7"), pAsset);
			return WireFrame();
		}

		UI64 vertexSize = 0;
		for (auto& attribute : vertexAttributes)
			vertexSize += attribute.mSize;

		for (UI32 i = 0; i < pScene->mNumMeshes; i++)
		{
			auto pMesh = pScene->mMeshes[i];

			Backend::Buffer* pVertexBuffer = GetDevice()->CreateBuffer(vertexSize * pMesh->mNumVertices, Backend::BufferUsage::VERTEX, Backend::MemoryProfile::TRANSFER_FRIENDLY);
			Backend::Buffer* pIndexBuffer = nullptr;

			void* pDataStore = pVertexBuffer->MapMemory(vertexSize * pMesh->mNumVertices, 0);
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
							std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::NORMAL:
						if (pMesh->HasNormals())
							std::copy(&pMesh->mNormals[j].x, (&pMesh->mNormals[j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::COLOR_0:
						if (pMesh->HasVertexColors(0))
							std::copy(&pMesh->mColors[0][j].r, (&pMesh->mColors[0][j].r) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::COLOR_1:
						if (pMesh->HasVertexColors(1))
							std::copy(&pMesh->mColors[1][j].r, (&pMesh->mColors[1][j].r) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::COLOR_2:
						if (pMesh->HasVertexColors(2))
							std::copy(&pMesh->mColors[2][j].r, (&pMesh->mColors[0][j].r) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::COLOR_3:
						if (pMesh->HasVertexColors(3))
							std::copy(&pMesh->mColors[3][j].r, (&pMesh->mColors[0][j].r) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_0:
						if (pMesh->HasTextureCoords(0))
							std::copy(&pMesh->mTextureCoords[0][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_1:
						if (pMesh->HasTextureCoords(1))
							std::copy(&pMesh->mTextureCoords[1][j].x, (&pMesh->mTextureCoords[1][j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_2:
						if (pMesh->HasTextureCoords(2))
							std::copy(&pMesh->mTextureCoords[2][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_3:
						if (pMesh->HasTextureCoords(3))
							std::copy(&pMesh->mTextureCoords[3][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_4:
						if (pMesh->HasTextureCoords(4))
							std::copy(&pMesh->mTextureCoords[4][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_5:
						if (pMesh->HasTextureCoords(5))
							std::copy(&pMesh->mTextureCoords[5][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_6:
						if (pMesh->HasTextureCoords(6))
							std::copy(&pMesh->mTextureCoords[6][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::TEXTURE_COORDINATES_7:
						if (pMesh->HasTextureCoords(7))
							std::copy(&pMesh->mTextureCoords[7][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::UV_COORDINATES:
						if (pMesh->HasPositions())
							std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::TANGENT:
						if (pMesh->HasPositions())
							std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::BITANGENT:
						if (pMesh->HasPositions())
							std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::BONE_ID:
						if (pMesh->HasPositions())
							std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::BONE_WEIGHT:
						if (pMesh->HasPositions())
							std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, static_cast<float*>(pDataStore));
						break;

					case Flint::VertexAttributeType::CUSTOM:
						break;

					default:
						FLINT_LOG_ERROR(TEXT("Invalid or Undefined vertex attribute type!"));
						break;
					}

					pDataStore = IncrementPointer(pDataStore, attribute.mSize);
				}
			}

			pVertexBuffer->UnmapMemory();
		}

		return WireFrame();
	}
}
