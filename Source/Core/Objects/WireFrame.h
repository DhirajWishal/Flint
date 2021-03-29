// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"
#include "Core/Backend/Buffer.h"

#include "VulkanBackend\VulkanExporter.h"

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
		WireFrame() {}
		WireFrame(const WireFrame& other);
		WireFrame(WireFrame&& other) noexcept;

		/**
		 * Clear the wire frame information.
		 * This terminates the vertex and index buffers if created.
		 */
		void Clear();

		Objects::Buffer mVertexBuffer = {};
		Objects::Buffer mIndexBuffer = {};

		std::vector<VertexAttribute> mAttributes;

	public:
		/**
		 * Load the wire frame content from a cache file.
		 * This is much faster than loading from a object file.
		 * 
		 * @param asset: The cache file to be loaded from.
		 * @param pDevice: The device pointer.
		 */
		void LoadFromCache(std::filesystem::path asset, Objects::Device* pDevice);

		/**
		 * Create a cache file using the wire frame content.
		 * 
		 * @param title: The title of the file.
		 */
		void CreateCache(std::filesystem::path title);

	public:
		/**
		 * Assignment operator (copy).
		 * 
		 * @param other: The other wire frame object to copy data from.
		 */
		WireFrame& operator=(const WireFrame& other);

		/**
		 * Assignment operator (move).
		 *
		 * @param other: The other wire frame object to move data from.
		 */
		WireFrame& operator=(WireFrame&& other) noexcept;

	public:
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