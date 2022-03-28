// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Image.hpp"
#include "GraphicsCore/MaterialProperties.hpp"

namespace Flint
{
	/**
	 * Flint image loader.
	 */
	class ImageLoader
	{
	public:
		ImageLoader() = default;

		/**
		 * Construct the loader by loading the image.
		 *
		 * @param assetFile The image file to load.
		 */
		ImageLoader(const std::filesystem::path& assetFile);

		/**
		 * Construct the loader by loading multiple images.
		 *
		 * @param assetFiles The image files to load.
		 */
		ImageLoader(const std::vector<std::filesystem::path>& assetFiles);

		ImageLoader(const ImageLoader&) = default;
		ImageLoader(ImageLoader&& other);

		/**
		 * Default destructor.
		 */
		~ImageLoader();

		/**
		 * Create the image using the loaded data.
		 *
		 * @param pDevice The device pointer.
		 * @param type The image type.
		 * @param usage The image usage.
		 * @param layers The image layers.
		 * @param mipLevels The number of mip levels to use. If set to 0, the best mip level is set.
		 * @param sampleCount The multi sample count. Default is One.
		 * @return The image pointer.
		 */
		std::unique_ptr<Image> CreateImage(Device* pDevice, const ImageType type, const ImageUsage usage, const uint32_t layers, const uint32_t mipLevels, const MultiSampleCount sampleCount = MultiSampleCount::One) const;

		/**
		 * Get the image as texture data.
		 * This will transfer ownership.
		 * 
		 * @param type The type of the image.
		 * @return The texture data.
		 */
		MaterialProperties::TextureData GetAsTextureData(const MaterialProperties::TextureType type);

	public:
		ImageLoader& operator=(const ImageLoader&) = default;
		ImageLoader& operator=(ImageLoader && other);

	public:
		/**
		 * Get the extent of the image.
		 *
		 * @return The extent.
		 */
		const FBox3D GetExtent() const { return m_Extent; }

		/**
		 * Get the pixel data.
		 *
		 * @return The pixel data pointer.
		 */
		const unsigned char* GetPixelData() const { return pPixelData; }

		/**
		 * Get the pixel format.
		 *
		 * @return The format.
		 */
		const PixelFormat GetPixelFormat() const { return m_PixelFormat; }

	private:
		FBox3D m_Extent = {};
		unsigned char* pPixelData = nullptr;
		PixelFormat m_PixelFormat = PixelFormat::Undefined;
	};
}