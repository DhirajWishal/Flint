// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Flint image object.
	 * This object is used to store information about a single image. This can have multiple levels of the same image.
	 */
	class Image : public DeviceBoundObject
	{
	public:
		/**
		 * Construct the image.
		 *
		 * @param pDevice: The device pointer.
		 * @param type: The image type.
		 * @param usage: The image usage.
		 * @param extent: The image extent.
		 * @param format: The image format.
		 * @param layers: The number of layers in the image.
		 * @param mipLevels: The mip levels.
		 * @param pImageData: The image data pointer to load data from.
		 */
		Image(const std::shared_ptr<Device>& pDevice, ImageType type, ImageUsage usage, const FBox3D& extent, PixelFormat format, UI8 layers, UI32 mipLevels, const void* pImageData);

	public:
		/**
		 * Get the extent of the image.
		 *
		 * @return The extent.
		 */
		const FBox3D GetExtent() const { return mExtent; }

		/**
		 * Get the mip levels of the current image.
		 * 
		 * @return The mip levels.
		 */
		const UI32 GetMipLevels() const { return mMipLevels; }

		/**
		 * Get the type of the image.
		 *
		 * @return The type.
		 */
		const ImageType GetType() const { return mType; }

		/**
		 * Get the image usage.
		 *
		 * @return The usage.
		 */
		const ImageUsage GetUsage() const { return mUsage; }

		/**
		 * Get the format of the image.
		 *
		 * @return The format.
		 */
		const PixelFormat GetFormat() const { return mFormat; }

		/**
		 * Get the layer count of the image.
		 *
		 * @return The layer count.
		 */
		const UI8 GetLayerCount() const { return mLayerCount; }

	protected:
		FBox3D mExtent = {};
		UI32 mMipLevels = 0;
		ImageType mType = ImageType::DIMENSIONS_2;
		ImageUsage mUsage = ImageUsage::GRAPHICS;
		PixelFormat mFormat = PixelFormat::UNDEFINED;
		UI8 mLayerCount = 1;
	};
}