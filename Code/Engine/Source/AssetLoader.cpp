// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/AssetLoader.hpp"

namespace Flint
{
	AssetLoader::AssetLoader(const std::shared_ptr<GeometryStore>& pGeometryStore, const VertexDescriptor& vertexDescriptor)
		: pGeometryStore(pGeometryStore), mDescriptor(vertexDescriptor)
	{
	}
}