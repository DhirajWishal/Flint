// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TextureView.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Texture 2D class.
		 */
		class Texture2D : public Texture
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
			 * @param mipLevels The texture's mip levels. If this is set to 0 (default), the best value is set.
			 * @param multisampleCount The multisample count of the image. Default is one.
			 * @param pDataStore The data store pointer to load the data from. Default is nullptr.
			 */
			explicit Texture2D(const std::shared_ptr<Device>& pDevice, uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format, uint32_t mipLevels = 0, Multisample multisampleCount = Multisample::One, const std::byte* pDataStore = nullptr);

			/**
			 * Default virtual destructor.
			 */
			virtual ~Texture2D() = default;

			/**
			 * Load a texture from an asset file.
			 *
			 * @param pDevice The device pointer.
			 * @param assetFile The asset file path.
			 * @param usage The image usage.
			 * @param multisampleCount The multisample count. Default is one.
			 * @return The texture pointer.
			 */
			[[nodiscard]] static std::shared_ptr<Texture2D> LoadFromFile(const std::shared_ptr<Device>& pDevice, const std::filesystem::path& assetFile, ImageUsage usage, Multisample multisampleCount = Multisample::One);
		};
	}
}