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
		 * Default destructor.
		 */
		~ImageLoader();

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