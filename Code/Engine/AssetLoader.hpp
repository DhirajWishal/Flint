// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GeometryStore.hpp"

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
		INTEGRITY,
		BONE_ID,
		BONE_WEIGHT,
		CUSTOM
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
		 * @param vertexDescriptor: The vertex descriptor describing the vertex attributes and loading order.
		 */
		AssetLoader(const std::shared_ptr<GeometryStore>& pGeometryStore, const VertexDescriptor& vertexDescriptor);

	private:
		std::shared_ptr<GeometryStore> pGeometryStore = nullptr;
		VertexDescriptor mDescriptor = {};
	};
}