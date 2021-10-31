// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphics/Tools/AssetLoader.hpp"

namespace Flint
{
	/**
	 * Flint asset object.
	 * These objects store a single asset along with its wire frames.
	 */
	class Asset
	{
	public:
		/**
		 * Default constructor.
		 */
		Asset() = default;

		/**
		 * Construct the asset using the wire frame and geometry store.
		 * 
		 * @param wireFrames The wire frames.
		 * @param pGeometryStore The geometry store pointer.
		 */
		Asset(const std::vector<WireFrame>& wireFrames, const std::shared_ptr<GeometryStore>& pGeometryStore);

		/**
		 * Load the asset from a file.
		 * 
		 * @param assetFile The asset file path.
		 * @param pGeometryStore The geometry store pointer.
		 * @param vertexDescriptor The vertex descriptor stating the vertex data to load.
		 * @param useMultiThreading Use multi threading to load the assets.
		 */
		Asset(const std::filesystem::path& assetFile, const std::shared_ptr<GeometryStore>& pGeometryStore, const VertexDescriptor& vertexDescriptor, const bool useMultiThreading = true);

		/**
		 * Clear the asset data.
		 * This will not terminate the geometry store as it might be in use by some other asset.
		 */
		void Clear();

	public:
		/**
		 * Get the wire frames stored in the asset.
		 *
		 * @return The wire frames.
		 */
		const std::vector<WireFrame> GetWireFrames() const { return mWireFrames; }

		/**
		 * Get the wire frames stored in the asset.
		 *
		 * @return The wire frames.
		 */
		std::vector<WireFrame>& GetWireFrames() { return mWireFrames; }

		/**
		 * Get the geometry store in which the asset is stored.
		 *
		 * @return The geometry store pointer.
		 */
		const std::shared_ptr<GeometryStore> GetGeometryStore() const { return pGeometryStore; }

	private:
		std::vector<WireFrame> mWireFrames = {};
		std::shared_ptr<GeometryStore> pGeometryStore = nullptr;
	};
}