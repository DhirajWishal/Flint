// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Texture 2D class.
	 */
	class Texture2D : public DeviceBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device to which this object is bound to.
		 * @param width The width of the image.
		 * @param height The height of the image.
		 * @param usage The image usage.
		 * @param format The image's pixel format.
		 */
		explicit Texture2D(const std::shared_ptr<Device>& pDevice, uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format);

		/**
		 * Get the width of the texture.
		 *
		 * @return The width.
		 */
		[[nodiscard]] uint32_t getWidth() const { return m_Width; }

		/**
		 * Get the height of the texture.
		 *
		 * @return The height.
		 */
		[[nodiscard]] uint32_t getHeight() const { return m_Height; }

		/**
		 * Get the image usage.
		 *
		 * @return The image usage.
		 */
		[[nodiscard]] ImageUsage getImageUsage() const { return m_Usage; }

		/**
		 * Get the pixel format.
		 *
		 * @return The format.
		 */
		[[nodiscard]] PixelFormat getPixelFormat() const { return m_Format; }

	protected:
		const uint32_t m_Width = 0;
		const uint32_t m_Height = 0;
		const ImageUsage m_Usage = ImageUsage::Graphics;
		const PixelFormat m_Format = PixelFormat::Undefined;
	};
}