// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Asset.hpp"

namespace Flint
{
	Asset::Asset(const std::vector<WireFrame>& wireFrames, const std::shared_ptr<GeometryStore>& pGeometryStore)
		: mWireFrames(wireFrames), pGeometryStore(pGeometryStore)
	{
	}

	Asset::Asset(const std::filesystem::path& assetFile, const std::shared_ptr<GeometryStore>& pGeometryStore, const VertexDescriptor& vertexDescriptor)
		: pGeometryStore(pGeometryStore)
	{
		AssetLoader loader(pGeometryStore, assetFile, vertexDescriptor);
		mWireFrames = loader.GetWireFrames();
	}
}