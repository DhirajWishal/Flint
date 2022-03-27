// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/WireFrame.hpp"

namespace Flint
{
	/**
	 * Input attribute type enum.
	 */
	enum class InputAttributeType : uint8_t
	{
		Undefined,
		Position,
		Normal,

		ColorZero,
		ColorOne,
		ColorTwo,
		ColorThree,

		TextureCoordinatesZero,
		TextureCoordinatesOne,
		TextureCoordinatesTwo,
		TextureCoordinatesThree,
		TextureCoordinatesFour,
		TextureCoordinatesFive,
		TextureCoordinatesSix,
		TextureCoordinatesSeven,

		UVCoordinates,
		Tangent,
		Bitangent,

		BoneID,
		BoneWeight,
	};

	/**
	 * Vertex attribute structure.
	 */
	struct VertexAttribute
	{
		VertexAttribute() = default;
		VertexAttribute(uint8_t size, InputAttributeType type) : mAttributeSize(size), mType(type) {}

		uint8_t mAttributeSize = 0;
		InputAttributeType mType = InputAttributeType::Undefined;
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
		const uint64_t Size() const;
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
		 * @param pGeometryStore The geometry store pointer to store the geometry data to.
		 * @param assetPath The asset file to load.
		 * @param vertexDescriptor The vertex descriptor describing the vertex attributes and loading order.
		 * @param useMultiThreading Use multi threading to load the assets.
		 */
		AssetLoader(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::filesystem::path& assetPath, const VertexDescriptor& vertexDescriptor, const bool useMultiThreading = true);

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

		/**
		 * Create the sky box vertex descriptor.
		 * The default descriptor contains,
		 * 1. Position (float 3).
		 * 2. Texture Coordinates (float 2).
		 */
		VertexDescriptor CreateSkyBoxVertexDescriptor();
	}
}