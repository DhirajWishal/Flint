// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"
#include "Core/Backend/Buffer.h"

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
		VertexAttributeType mType = VertexAttributeType::UNDEFINED;
		UI64 mSize = 0;
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
		WireFrame(Backend::Buffer* pVertex, Backend::Buffer* pIndex) : pVertexBuffer(pVertex), pIndexBuffer(pIndex) {}
		WireFrame(const String& name, Backend::Buffer* pVertex, Backend::Buffer* pIndex) 
			: mName(name), pVertexBuffer(pVertex), pIndexBuffer(pIndex) {}

		/**
		 * Clear the wire frame information.
		 * This terminates the vertex and index buffers if created.
		 */
		void Clear();

		String mName = {};

		Backend::Buffer* pVertexBuffer = nullptr;
		UI64 mVertexCount = 0;

		Backend::Buffer* pIndexBuffer = nullptr;
		UI64 mIndexCount = 0;

		std::vector<VertexAttribute> mAttributes;
	};
}