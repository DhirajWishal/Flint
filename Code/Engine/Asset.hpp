// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/AssetLoader.hpp"

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
		 * @param wireFrame: The wire frames.
		 * @param pGeometryStore: The geometry store pointer.
		 */
		Asset(const boost::container::vector<WireFrame>& wireFrames, const boost::shared_ptr<GeometryStore>& pGeometryStore);

		/**
		 * Load the asset from a file.
		 * 
		 * @param assetFile: The asset file path.
		 * @param pGeometryStore: The geometry store pointer.
		 * @param vertexDescriptor: The vertex descriptor stating the vertex data to load.
		 */
		Asset(const boost::filesystem::path& assetFile, const boost::shared_ptr<GeometryStore>& pGeometryStore, const VertexDescriptor& vertexDescriptor);

	public:
		/**
		 * Get the wire frames stored in the asset.
		 *
		 * @return The wire frames.
		 */
		const boost::container::vector<WireFrame> GetWireFrames() const { return mWireFrames; }

		/**
		 * Get the wire frames stored in the asset.
		 *
		 * @return The wire frames.
		 */
		boost::container::vector<WireFrame>& GetWireFrames() { return mWireFrames; }

		/**
		 * Get the geometry store in which the asset is stored.
		 *
		 * @return The geometry store pointer.
		 */
		const boost::shared_ptr<GeometryStore> GetGeometryStore() const { return pGeometryStore; }

	private:
		boost::container::vector<WireFrame> mWireFrames = {};
		boost::shared_ptr<GeometryStore> pGeometryStore = nullptr;
	};
}