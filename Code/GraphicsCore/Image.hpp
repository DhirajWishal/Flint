// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

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
		 * @param sampleCount: The multi sample count to use.
		 */
		Image(const std::shared_ptr<Device>& pDevice, const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const UI8 layers, const UI32 mipLevels, const void* pImageData, const MultiSampleCount sampleCount = MultiSampleCount::One);

		/**
		 * Generate mip maps.
		 */
		virtual void GenerateMipMaps() = 0;

		/**
		 * Copy the image content to a buffer.
		 *
		 * @return The stagging buffer.
		 */
		virtual std::shared_ptr<Buffer> CopyToBuffer() = 0;

		/**
		 * Get the value of a single pixel.
		 *
		 * @tparam Type: The type of the pixel.
		 * @param position: The position of the pixel.
		 * @return The pixel data.
		 */
		template<class Type>
		Type GetPixelValue(const FBox3D position)
		{
			std::shared_ptr<Buffer> pStagingBuffer = CopyToBuffer();
			Type* pPixels = static_cast<Type*>(pStagingBuffer->MapMemory(pStagingBuffer->GetSize()));

			UI64 row = static_cast<UI64>(position.mHeight) * mExtent.mWidth;
			UI64 index = row + position.mWidth;
			Type pixel = pPixels[index];

			pStagingBuffer->UnmapMemory();
			pStagingBuffer->Terminate();

			return pixel;
		}

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

	public:
		/**
		 * Get the best mip levels for an image.
		 * 
		 * @param extent: The extent of the image.
		 * @return The mip levels.
		 */
		static UI32 GetBestMipLevels(const FBox2D extent);

	protected:
		FBox3D mExtent = {};
		UI32 mMipLevels = 0;
		ImageType mType = ImageType::TwoDimension;
		ImageUsage mUsage = ImageUsage::Graphics;
		PixelFormat mFormat = PixelFormat::Undefined;
		MultiSampleCount mMultiSampleCount = MultiSampleCount::One;
		UI8 mLayerCount = 1;
	};
}