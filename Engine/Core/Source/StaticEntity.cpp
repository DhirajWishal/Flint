// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/StaticEntity.hpp"
#include "Core/Errors/AssetError.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Flint
{
	void StaticEntity::LoadModel(std::filesystem::path&& path)
	{
		// Setup the importer and the load the scene.
		Assimp::Importer importer = {};
		const aiScene* pScene = importer.ReadFile(path.string(),
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices |
			aiProcess_Triangulate |
			//aiProcess_OptimizeMeshes |
			//aiProcess_OptimizeGraph |
			aiProcess_SortByPType |
			aiProcess_GenUVCoords |
			aiProcess_FlipUVs);

		// If the scene was not loaded properly, we can throw an exception.
		if (!pScene)
			throw AssetError("Failed to load the specified model!");
	}
}