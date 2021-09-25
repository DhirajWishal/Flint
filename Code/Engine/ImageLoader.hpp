// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Image.hpp"

namespace Flint
{
	/**
	 * Flint image loader.
	 */
	class ImageLoader
	{
	public:
		/**
		 * Construct the loader by loading the image.
		 *
		 * @param assetFile: The image file to load.
		 */
		ImageLoader(const std::filesystem::path& assetFile);

		/**
		 * Construct the loader by loading multiple images.
		 *
		 * @param assetFiles: The image files to load.
		 */
		ImageLoader(const std::vector<std::filesystem::path>& assetFiles);

		/**
		 * Default destructor.
		 */
		~ImageLoader();

		/**
		 * Create the image using the loaded data.
		 *
		 * @param pDevice: The device pointer.
		 * @param type: The image type.
		 * @param usage: The image usage.
		 * @param layers: The image layers.
		 * @param mipLevels: The number of mip levels to use. If set to 0, the best mip level is set.
		 * @param sampleCount: The multi sample count. Default is One.
		 * @return The image pointer.
		 */
		std::shared_ptr<Image> CreateImage(const std::shared_ptr<Device>& pDevice, const ImageType type, const ImageUsage usage, const UI32 layers, const UI32 mipLevels, const MultiSampleCount sampleCount = MultiSampleCount::One) const;

	public:
		/**
		 * Get the extent of the image.
		 *
		 * @return The extent.
		 */
		const FBox3D GetExtent() const { return mExtent; }

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
		const PixelFormat GetPixelFormat() const { return mPixelFormat; }

	private:
		FBox3D mExtent = {};
		unsigned char* pPixelData = nullptr;
		PixelFormat mPixelFormat = PixelFormat::Undefined;
	};
}