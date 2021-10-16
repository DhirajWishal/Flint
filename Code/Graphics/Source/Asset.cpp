// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Asset.hpp"

namespace Flint
{
	Asset::Asset(const std::vector<WireFrame>& wireFrames, const std::shared_ptr<GeometryStore>& pGeometryStore)
		: mWireFrames(wireFrames), pGeometryStore(pGeometryStore)
	{
	}

	Asset::Asset(const std::filesystem::path& assetFile, const std::shared_ptr<GeometryStore>& pGeometryStore, const VertexDescriptor& vertexDescriptor, const bool useMultiThreading)
		: pGeometryStore(pGeometryStore)
	{
		AssetLoader loader(pGeometryStore, assetFile, vertexDescriptor, useMultiThreading);
		mWireFrames = loader.GetWireFrames();
	}
	
	void Asset::Clear()
	{
		mWireFrames.clear();
	}
}