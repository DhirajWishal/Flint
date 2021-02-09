// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Components/WireFrame.h"
#include "Core/ErrorHandler/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Flint
{
	void WireFrame::LoadFromFile(const char* pAsset, const std::vector<VertexAttribute>& attributes)
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
			return;
		}

		for (UI32 i = 0; i < pScene->mNumMeshes; i++)
		{
			auto pMesh = pScene->mMeshes[i];

			for (UI32 j = 0; j < pMesh->mNumVertices; j++)
			{
				auto pVertex = pMesh->mVertices[j];
				pVertex.Normalize();
			}
		}
	}
	
	std::vector<VertexAttribute> CreateDefaultAttributes()
	{
		std::vector<VertexAttribute> attributes(2);

		attributes[0].mType = VertexAttributeType::POSITION;
		attributes[0].mSize = sizeof(float) * 4;

		attributes[0].mType = VertexAttributeType::TEXTURE_COORDINATES_0;
		attributes[0].mSize = sizeof(float) * 2;

		return attributes;
	}
}