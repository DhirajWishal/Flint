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
}