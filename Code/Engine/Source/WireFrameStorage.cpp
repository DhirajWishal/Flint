// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "WireFrameStorage.hpp"
#include "Core/Error.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
		{
			//FLINT_LOG_ERROR(TEXT("Failed to load wire frame from asset path: #8"), file.string());
			FLINT_THROW_RUNTIME_ERROR("Failed to load wire frame form the asset file!");
			//return {};
		}

		//return WireFrame();
	}

	WireFrame WireFrameStorage::LoadFromCache(const std::filesystem::path& file)
	{
		//return WireFrame();
	}
}