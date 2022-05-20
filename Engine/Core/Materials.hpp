// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <filesystem>

namespace Flint
{
	/**
	 * Texture type enum.
	 */
	enum class TextureType : uint8_t
	{
		BaseColor,
		NormalCamera,
		EmissionColor,
		Metalness,
		DiffuseRoughness,
		AmbientOcclusion,
		SheenColor,
		SheenRoughness,
		ClearCoat,
		ClearCoatRoughness,
		ClearCoatNormal,
		Transmission,

		Unknown
	};

	/**
	 * Texture material.
	 */
	struct Texture final
	{
		/**
		 * Default constructor.
		 */
		constexpr Texture() = default;

		/**
		 * Explicit constructor.
		 *
		 * @param path The path of the texture.
		 * @param type The texture type.
		 */
		explicit Texture(std::filesystem::path&& path, TextureType type) : m_FilePath(std::move(path)), m_Type(type) {}

		std::filesystem::path m_FilePath;
		TextureType m_Type = TextureType::Unknown;
	};

	/**
	 * Color type enum.
	 */
	enum class ColorType : uint8_t
	{
		Diffuse,
		Ambient,
		Specular,
		Emissive,
		Transparent,
		Reflective,
		Base
	};

	/**
	 * Color material.
	 */
	struct Color final
	{
		/**
		 * Default constructor.
		 */
		constexpr Color() = default;

		/**
		 * Explicit constructor.
		 *
		 * @param r The red value.
		 * @param g The green value.
		 * @param b The blue value.
		 * @param a The alpha value.
		 * @param type The type of the color.
		 */
		explicit Color(float r, float g, float b, float a, ColorType type)
			: m_Type(type)
		{
			m_Components[0] = r;
			m_Components[1] = g;
			m_Components[2] = b;
			m_Components[3] = a;
		}

		float m_Components[4] = {};
		ColorType m_Type = ColorType::Diffuse;
	};
}