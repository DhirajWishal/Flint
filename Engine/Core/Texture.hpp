// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

namespace Flint
{
	class TextureView;

	/**
	 * Texture class.
	 * Textures are just images in the GPU.
	 */
	class Texture : public DeviceBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param width The width of the image.
		 * @param height The height of the image.
		 * @param usage The image usage.
		 * @param format The image's pixel format.
		 * @param mipLevels The texture's mip levels. If this is set to 0 (default), the best value is set.
		 * @param multisampleCount The multisample count of the image. Default is one.
		 */
		explicit Texture(const std::shared_ptr<Device>& pDevice, uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format, uint32_t mipLevels = 0, Multisample multisampleCount = Multisample::One);

		/**
		 * Default virtual destructor.
		 */
		virtual ~Texture() = default;

		/**
		 * Create the image view for the current texture.
		 *
		 * @return The texture view.
		 */
		[[nodiscard]] virtual std::shared_ptr<TextureView> createView() = 0;

		/**
		 * Copy the texture image to a buffer.
		 *
		 * @return The staging buffer containing the image.
		 */
		[[nodiscard]] virtual std::shared_ptr<Buffer> toBuffer() const = 0;

		/**
		 * Copy the image data from a buffer.
		 *
		 * @param pBuffer The buffer to load the data from.
		 */
		virtual void copyFrom(const Buffer* pBuffer) = 0;

		/**
		 * Copy the image data from a raw memory pointer.
		 * Make sure that the memory size is the same as the image size (width * height * pixel size).
		 *
		 * @param pDataStore The data store to load the data from.
		 */
		virtual void copyFrom(const std::byte* pDataStore) = 0;

		/**
		 * Copy the image from another texture.
		 *
		 * @param pTexture The texture object pointer to copy from.
		 */
		virtual void copyFrom(const Texture* pTexutre) = 0;

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

		/**
		 * Get the mip levels.
		 *
		 * @return The mip levels.
		 */
		[[nodiscard]] uint32_t getMipLevels() const { return m_MipLevels; }

	protected:
		const uint32_t m_Width = 0;
		const uint32_t m_Height = 0;
		const uint32_t m_MipLevels = 0;
		const ImageUsage m_Usage = ImageUsage::Graphics;
		const PixelFormat m_Format = PixelFormat::Undefined;
		const Multisample m_MultisampleCount = Multisample::One;
	};
}