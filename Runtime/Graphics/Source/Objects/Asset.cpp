// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Objects/Asset.hpp"

namespace Flint
{
	Asset::Asset(const std::vector<WireFrame>& wireFrames, const std::shared_ptr<VulkanBackend::VulkanGeometryStore>& pGeometryStore)
		: m_WireFrames(wireFrames), pGeometryStore(pGeometryStore)
	{
	}

	Asset::Asset(const std::filesystem::path& assetFile, const std::shared_ptr<VulkanBackend::VulkanGeometryStore>& pGeometryStore, const VertexDescriptor& vertexDescriptor, const bool useMultiThreading)
		: pGeometryStore(pGeometryStore)
	{
		AssetLoader loader(pGeometryStore, assetFile, vertexDescriptor, useMultiThreading);
		m_WireFrames = loader.GetWireFrames();
	}
	
	void Asset::Clear()
	{
		m_WireFrames.clear();
	}
}