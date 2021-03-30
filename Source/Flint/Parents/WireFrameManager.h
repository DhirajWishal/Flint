// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsBackendObject.h"
#include "Flint\Components\WireFrame.h"

namespace Flint
{
	/**
	 * Wire frame manager.
	 * This object is responsible of creating, manging and manipulating Wire Frame objects.
	 */
	class WireFrameManager : public GraphicsBackendObject {
	public:
		WireFrameManager();
		~WireFrameManager();

		/**
		 * Create a new wire frame objects.
		 *
		 * @param pAsset: The asset file to load the data from.
		 * @param vertexAttributes: The vertex attributes to be loaded from the file.
		 * @return The created wire frame object.
		 */
		WireFrame CreateNewWireFrame(const char* pAsset, std::vector<VertexAttribute>& vertexAttributes);

		Objects::Image LoadImageData(std::filesystem::path filePath);
	};
}