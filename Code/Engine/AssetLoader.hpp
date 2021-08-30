// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/WireFrame.hpp"

namespace Flint
{
	/**
	 * Vertex attribute type enum.
	 */
	enum class VertexAttributeType : UI8 {
		UNDEFINED,
		POSITION,
		NORMAL,

		COLOR_0,
		COLOR_1,
		COLOR_2,
		COLOR_3,

		TEXTURE_COORDINATES_0,
		TEXTURE_COORDINATES_1,
		TEXTURE_COORDINATES_2,
		TEXTURE_COORDINATES_3,
		TEXTURE_COORDINATES_4,
		TEXTURE_COORDINATES_5,
		TEXTURE_COORDINATES_6,
		TEXTURE_COORDINATES_7,

		UV_COORDINATES,
		TANGENT,
		BITANGENT,

		BONE_ID,
		BONE_WEIGHT,
	};

	/**
	 * Vertex attribute structure.
	 */
	struct VertexAttribute
	{
		VertexAttribute() = default;
		VertexAttribute(UI8 size, VertexAttributeType type) : mAttributeSize(size), mType(type) {}

		UI8 mAttributeSize = 0;
		VertexAttributeType mType = VertexAttributeType::UNDEFINED;
	};

	/**
	 * Vertex descriptor structure.
	 * This structure contains information about a single vertex.
	 */
	struct VertexDescriptor
	{
		std::vector<VertexAttribute> mAttributes = {};

		/**
		 * Get the size of the vertex.
		 * 
		 * @return The size in bytes.
		 */
		const UI64 Size() const;
	};

	/**
	 * Flint asset loader object.
	 * This object is used to load assets to the engine.
	 */
	class AssetLoader
	{
	public:
		/**
		 * Construct the loader.
		 *
		 * @param pGeometryStore: The geometry store pointer to store the geometry data to.
		 * @param assetPath: The asset file to load.
		 * @param vertexDescriptor: The vertex descriptor describing the vertex attributes and loading order.
		 */
		AssetLoader(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::filesystem::path& assetPath, const VertexDescriptor& vertexDescriptor);

		/**
		 * Get the loaded wire frames.
		 * 
		 * @return The vector of wire frames.
		 */
		const std::vector<WireFrame> GetWireFrames() const { return mWireFrames; }

	private:
		std::vector<WireFrame> mWireFrames = {};

		std::shared_ptr<GeometryStore> pGeometryStore = nullptr;
		VertexDescriptor mDescriptor = {};
	};

	namespace Defaults
	{
		/**
		 * Create the default vertex descriptor.
		 * The default descriptor contains,
		 * 1. Position (float 3).
		 * 2. Color (float 3).
		 * 3. Texture Coordinates (float 2).
		 */
		VertexDescriptor CreateDefaultVertexDescriptor();
	}
}