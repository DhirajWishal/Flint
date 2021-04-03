// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "WireFrameManager.h"

namespace Flint
{
	/**
	 * Flint Asset Registry.
	 * This object manages creation, termination and basic utility functions that can be performed on Flint assets.
	 */
	class AssetRegistry : public WireFrameManager {
	public:
		AssetRegistry() {}
		~AssetRegistry() {}

		WireFrame* CreateWireFrame(const String& name, const std::filesystem::path& asset, std::vector<VertexAttribute>& vertexAttributes);

	private:
		std::unordered_map<String, WireFrame> mWireFrames;
	};
}