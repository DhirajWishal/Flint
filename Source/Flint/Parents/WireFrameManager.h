// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsBackendObject.h"
#include "Flint\Components\WireFrame.h"

#include "VulkanBackend\VulkanExporter.h"

namespace Flint
{
	/**
	 * Wire frame manager.
	 * This object is responsible of creating, manging and manipulating Wire Frame objects.
	 */
	class WireFrameManager : public GraphicsBackendObject {
	public:
		WireFrameManager();
		virtual ~WireFrameManager();

		/**
		 * Create a new wire frame objects.
		 *
		 * @param pAsset: The asset file to load the data from.
		 * @param vertexAttributes: The vertex attributes to be loaded from the file.
		 * @return The created wire frame object.
		 */
		WireFrame CreateNewWireFrame(std::filesystem::path asset, std::vector<VertexAttribute>& vertexAttributes);

		Image LoadImageData(std::filesystem::path filePath);
	};
}