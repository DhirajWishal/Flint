// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\FBuffer.h"

#include <filesystem>

namespace Flint
{
	enum class VertexAttributeType : UI8 {
		UNDEFINED,
		POSITION,
		NORMAL,
		COLOR_0, COLOR_1, COLOR_2, COLOR_3,
		TEXTURE_COORDINATES_0, TEXTURE_COORDINATES_1, TEXTURE_COORDINATES_2, TEXTURE_COORDINATES_3,
		TEXTURE_COORDINATES_4, TEXTURE_COORDINATES_5, TEXTURE_COORDINATES_6, TEXTURE_COORDINATES_7,
		UV_COORDINATES,
		TANGENT, BITANGENT,
		BONE_ID, BONE_WEIGHT,
		CUSTOM
	};

	struct VertexAttribute {
		UI64 mSize = 0;
		VertexAttributeType mType = VertexAttributeType::UNDEFINED;
	};

	/**
	 * Create default vertex attributes.
	 *
	 * Default attributes are comprised of,
	 * * Position (vec3)
	 */
	std::vector<VertexAttribute> CreateDefaultAttributes();

	/**
	 * Wire frame object.
	 * This object holds the geometry information about a single object.
	 */
	class WireFrame {
	public:
		WireFrame() = default;

		/**
		 * Load wire frame data from an external asset file.
		 * 
		 * @param pDevice: The device pointer.
		 * @param asset: The asset file.
		 * @param vertexAttributes: The vertex attributes to be loaded.
		 */
		WireFrame(std::shared_ptr<FDevice> pDevice, std::filesystem::path asset, std::vector<VertexAttribute> vertexAttributes);

		/**
		 * Load wire frame data form a wire frame cache file.
		 * 
		 * @param pDevice: The device pointer.
		 * @param asset: The asset file.
		 */
		WireFrame(std::shared_ptr<FDevice> pDevice, std::filesystem::path asset);

		/**
		 * Clear the wire frame information.
		 * This terminates the vertex and index buffers if created.
		 */
		void Clear();

		std::unique_ptr<FBuffer> pVertexBuffer = nullptr;
		std::unique_ptr<FBuffer> pIndexBuffer = nullptr;

		std::vector<VertexAttribute> mAttributes;

	public:
		/**
		 * Load wire frame content from an external file.
		 * 
		 * @param pDevice: The device pointer.
		 * @param asset: The asset file.
		 * @param vertexAttributes: The vertex attributes to be loaded.
		 */
		void LoadFromFile(std::shared_ptr<FDevice> pDevice, std::filesystem::path asset, std::vector<VertexAttribute> vertexAttributes);

		/**
		 * Load the wire frame content from a cache file.
		 * This is much faster than loading from a object file.
		 * 
		 * @param pDevice: The device object pointer.
		 * @param asset: The cache file to be loaded from.
		 */
		void LoadFromCache(std::shared_ptr<FDevice> pDevice, std::filesystem::path asset);

		/**
		 * Create a cache file using the wire frame content.
		 * 
		 * @param title: The title of the file.
		 */
		void CreateCache(std::filesystem::path title);

	public:
		/**
		 * Draw data container struct.
		 * This struct contains information about a single draw data.
		 */
		struct DrawData {
			String mName;

			UI64 mVertexOffset = 0;
			UI64 mVertexCount = 0;

			UI64 mIndexOffset = 0;
			UI64 mIndexCount = 0;
		};

		std::vector<DrawData> mDrawData;
	};
}