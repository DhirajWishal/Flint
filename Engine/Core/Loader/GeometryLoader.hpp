// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Geometry.hpp"

namespace Flint
{
	/**
	 * Vertex data enum.
	 * This can be used to define what type of data to load.
	 */
	enum class VertexData : uint32_t
	{
		Position = 1 << 0,
		Normal = 1 << 1,

		Tangent = 1 << 2,
		BiTangent = 1 << 3,

		Color0 = 1 << 4,
		Color1 = 1 << 5,
		Color2 = 1 << 6,
		Color3 = 1 << 7,
		Color4 = 1 << 8,
		Color5 = 1 << 9,
		Color6 = 1 << 10,
		Color7 = 1 << 11,

		Texture0 = 1 << 12,
		Texture1 = 1 << 13,
		Texture2 = 1 << 14,
		Texture3 = 1 << 15,
		Texture4 = 1 << 16,
		Texture5 = 1 << 17,
		Texture6 = 1 << 18,
		Texture7 = 1 << 19,

		All = Position | Normal | Tangent | BiTangent | Color0 | Color1 | Color2 | Color3 | Color4 | Color5 | Color6 | Color7 | Texture0 | Texture1 | Texture2 | Texture3 | Texture4 | Texture5 | Texture6 | Texture7
	};

	FLINT_DEFINE_ENUM_AND_OR(VertexData);

	/**
	 * Load geometry from a file.
	 *
	 * @param store The geometry store to load the geometry to.
	 * @param path The path of the asset file.
	 * @param vertexData The vertex data to load. It will only load them if available. By default it is all.
	 * @return The loaded geometry.
	 */
	Geometry LoadGeometry(GeometryStore& store, std::filesystem::path&& path, VertexData vertexData = VertexData::All);
}