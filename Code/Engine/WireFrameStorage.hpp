// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

#include "Registries/Components/WireFrame.hpp"

namespace Flint
{
	/**
	 * Flint wire frame storage object.
	 * This object contains a vertex and index buffer and is responsible of loading, maintaining and unloading vertex and index data.
	 */
	class WireFrameStorage final : public DeviceBoundObject
	{
	public:
		WireFrameStorage(Device& device, UI64 ID, const VertexDescriptor& descriptor, UI64 indexStride)
			: DeviceBoundObject(device), mID(ID), mVertexDescriptor(descriptor), mIndexStride(indexStride) {}

		/**
		 * Load a wire frame from an asset file.
		 *
		 * @param file: The file path.
		 * @return The wire frame object.
		 */
		WireFrame LoadFromAssetFile(const std::filesystem::path& file);

		/**
		 * Load a wire frame from an asset cache file.
		 *
		 * @param file: The file path.
		 * @return The wire frame object.
		 */
		WireFrame LoadFromCache(const std::filesystem::path& file);

	protected:
		VertexDescriptor mVertexDescriptor = {};

		std::unique_ptr<VertexBuffer> pVertexBuffer = nullptr;
		std::unique_ptr<IndexBuffer> pIndexBuffer = nullptr;

		UI64 mIndexStride = 0;
		UI64 mID = 0;
	};
}