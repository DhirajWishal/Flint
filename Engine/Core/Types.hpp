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
	 * Data type enum.
	 */
	enum class DataType : uint8_t
	{
		None = 0,								// Default type.

		R8G8B8 = sizeof(uint8_t[3]),			// 8 bit R, G and B components.
		R8G8B8A8 = sizeof(uint8_t[4]),			// 8 bit R, G, B and A components.

		R32G32B32 = sizeof(uint32_t[3]),		// 32 bit R, G and B components.
		R32G32B32A32 = sizeof(uint32_t[4]),		// 32 bit R, G, B and A components.

		U8V8 = sizeof(uint8_t[2]),				// 8 bit U and V components.
		U8V8W8 = sizeof(uint8_t[3]),			// 8 bit U, V and W components.

		U32V32 = sizeof(uint32_t[2]),			// 32 bit U and V components.
		U32V32W32 = sizeof(uint32_t[3]),		// 32 bit U, V and W components.

		X32Y32 = sizeof(uint32_t[2]),			// 32 bit X and Y components.
		X32Y32Z32 = sizeof(uint32_t[3]),		// 32 bit X, Y and Z components.
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
		DataType m_Position = DataType::None;		// Uses X, Y and Z.
		DataType m_Normal = DataType::None;			// Uses X, Y and Z.

		DataType m_Tangent = DataType::None;		// Uses X, Y and Z.
		DataType m_BiTangent = DataType::None;		// Uses X, Y and Z.

		DataType m_Color0 = DataType::None;			// Uses R, G, B and A.
		DataType m_Color1 = DataType::None;			// Uses R, G, B and A.
		DataType m_Color2 = DataType::None;			// Uses R, G, B and A.
		DataType m_Color3 = DataType::None;			// Uses R, G, B and A.
		DataType m_Color4 = DataType::None;			// Uses R, G, B and A.
		DataType m_Color5 = DataType::None;			// Uses R, G, B and A.
		DataType m_Color6 = DataType::None;			// Uses R, G, B and A.
		DataType m_Color7 = DataType::None;			// Uses R, G, B and A.

		DataType m_Texture0 = DataType::None;		// Uses U, V and W.
		DataType m_Texture1 = DataType::None;		// Uses U, V and W.
		DataType m_Texture2 = DataType::None;		// Uses U, V and W.
		DataType m_Texture3 = DataType::None;		// Uses U, V and W.
		DataType m_Texture4 = DataType::None;		// Uses U, V and W.
		DataType m_Texture5 = DataType::None;		// Uses U, V and W.
		DataType m_Texture6 = DataType::None;		// Uses U, V and W.
		DataType m_Texture7 = DataType::None;		// Uses U, V and W.
	};
}