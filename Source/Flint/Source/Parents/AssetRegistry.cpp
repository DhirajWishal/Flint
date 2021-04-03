// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Parents\AssetRegistry.h"

namespace Flint
{
	WireFrame* AssetRegistry::CreateWireFrame(const String& name, const std::filesystem::path& asset, std::vector<VertexAttribute>& vertexAttributes)
	{
		WireFrame wireFrame = CreateNewWireFrame(asset, vertexAttributes);

		mWireFrames[name] = std::move(wireFrame);
		return &mWireFrames[name];
	}
}