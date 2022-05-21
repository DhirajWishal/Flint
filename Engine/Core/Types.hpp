// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

namespace Flint
{
	/**
	 * Pixel format enum.
	 */
	enum class PixelFormat : uint8_t
	{
		Undefined,

		R8_SRGB,
		R8G8_SRGB,
		R8G8B8_SRGB,
		R8G8B8A8_SRGB,

		R8_UNORMAL,
		R8G8_UNORMAL,
		R8G8B8_UNORMAL,
		R8G8B8A8_UNORMAL,

		B8G8R8_SRGB,
		B8G8R8A8_SRGB,

		B8G8R8_UNORMAL,
		B8G8R8A8_UNORMAL,

		R16_SFLOAT,
		R16G16_SFLOAT,
		R16G16B16_SFLOAT,
		R16G16B16A16_SFLOAT,

		R32_SFLOAT,
		R32G32_SFLOAT,
		R32G32B32_SFLOAT,
		R32G32B32A32_SFLOAT,

		D16_SINT,
		D32_SFLOAT,

		S8_UINT,
		D16_UNORMAL_S8_UINT,
		D24_UNORMAL_S8_UINT,
		D32_SFLOAT_S8_UINT,
	};

	/**
	 * Attachment type enum.
	 */
	enum class AttachmentType : uint8_t
	{
		Color,
		Depth
	};

	/**
	 * Attachment description structure.
	 * This structure describes information about a single render target attachment.
	 */
	struct AttachmentDescription final
	{
		PixelFormat m_Format = PixelFormat::Undefined;	// If set to undefined, the best suitable will be set automatically.
		AttachmentType m_Type = AttachmentType::Color;
	};

	namespace Defaults
	{
		/**
		 * Color attachment description variable.
		 * This can be used to specify a default color attachment.
		 */
		constexpr auto ColorAttachmentDescription = AttachmentDescription{ PixelFormat::Undefined, AttachmentType::Color };

		/**
		 * Depth attachment description variable.
		 * This can be used to specify a default depth attachment.
		 */
		constexpr auto DepthAttachmentDescription = AttachmentDescription{ PixelFormat::Undefined, AttachmentType::Depth };
	}

	/**
	 * Position type enum.
	 */
	enum class PositionType : uint8_t
	{
		None = 0,	// Nothing is loaded.

		X32Y32 = sizeof(uint32_t[2]),
		X32Y32Z32 = sizeof(uint32_t[3]),
	};

	/**
	 * Coordinate type enum.
	 */
	enum class CoordinateType : uint8_t
	{
		None = 0,	// Nothing is loaded.

		R8G8B8 = sizeof(uint8_t[3]),
		R8G8B8A8 = sizeof(uint8_t[4]),

		R32G32B32 = sizeof(float[3]),
		R32G32B32A32 = sizeof(float[4]),

		U8V8 = sizeof(uint8_t[2]),
		U8V8W8 = sizeof(uint8_t[3]),

		U32V32 = sizeof(float[2]),
		U32V32W32 = sizeof(float[3]),
	};

	/**
	 * Vertex descriptor struct.
	 * This describes how to load the data.
	 *
	 * Note that attributes are processed/ packed in the following definition order. Unknown types will be packed last.
	 */
	struct VertexDescriptor final
	{
		/**
		 * Get the stride from the attribute types.
		 *
		 * @return The stride.
		 */
		[[nodiscard]] uint8_t getStride() const;

	public:
		PositionType m_Position = PositionType::None;		// Uses X, Y and Z.
		PositionType m_Normal = PositionType::None;			// Uses X, Y and Z.

		PositionType m_Tangent = PositionType::None;		// Uses X, Y and Z.
		PositionType m_BiTangent = PositionType::None;		// Uses X, Y and Z.

		CoordinateType m_Color0 = CoordinateType::None;		// Uses R, G, B and A.
		CoordinateType m_Color1 = CoordinateType::None;		// Uses R, G, B and A.
		CoordinateType m_Color2 = CoordinateType::None;		// Uses R, G, B and A.
		CoordinateType m_Color3 = CoordinateType::None;		// Uses R, G, B and A.
		CoordinateType m_Color4 = CoordinateType::None;		// Uses R, G, B and A.
		CoordinateType m_Color5 = CoordinateType::None;		// Uses R, G, B and A.
		CoordinateType m_Color6 = CoordinateType::None;		// Uses R, G, B and A.
		CoordinateType m_Color7 = CoordinateType::None;		// Uses R, G, B and A.

		CoordinateType m_Texture0 = CoordinateType::None;	// Uses U, V and W.
		CoordinateType m_Texture1 = CoordinateType::None;	// Uses U, V and W.
		CoordinateType m_Texture2 = CoordinateType::None;	// Uses U, V and W.
		CoordinateType m_Texture3 = CoordinateType::None;	// Uses U, V and W.
		CoordinateType m_Texture4 = CoordinateType::None;	// Uses U, V and W.
		CoordinateType m_Texture5 = CoordinateType::None;	// Uses U, V and W.
		CoordinateType m_Texture6 = CoordinateType::None;	// Uses U, V and W.
		CoordinateType m_Texture7 = CoordinateType::None;	// Uses U, V and W.
	};
}