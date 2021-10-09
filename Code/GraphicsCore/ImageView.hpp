// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Image.hpp"

namespace Flint
{
	/**
	 * Flint image view object.
	 * Image views are used to bind images to the resource packages.
	 */
	class ImageView : public DeviceBoundObject
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param pImage The image pointer.
		 * @param baseLayerIndex The index of the base layer.
		 * @param layerCount The number of layers the image view covers.
		 * @param baseMipLevel The base mip level.
		 * @param mipLevels The number of mip levels to cover.
		 * @param usage The image view usage.
		 */
		ImageView(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Image>& pImage, const UI32 baseLayerIndex, const UI32 layerCount, const UI32 baseMipLevel, const UI32 mipLevels, const ImageUsage usage);

	public:
		/**
		 * Get the parent image pointer.
		 *
		 * @return The image pointer.
		 */
		std::shared_ptr<Image> GetParentImage() const { return pParentImage; }

		/**
		 * Get the base layer index.
		 *
		 * @return The layer index.
		 */
		UI32 GetBaseLayerIndex() const { return mBaseLayerIndex; }

		/**
		 * Get the layer count.
		 *
		 * @return The layer count.
		 */
		UI32 GetLayerCount() const { return mLayerCount; }

		/**
		 * Get the base mip level.
		 *
		 * @return The mip level
		 */
		UI32 GetBaseMipLevel() const { return mBaseMipLevel; }

		/**
		 * Get the mip levels.
		 *
		 * @return The mip levels.
		 */
		UI32 GetMipLevels() const { return mMipLevels; }

	protected:
		std::shared_ptr<Image> pParentImage = nullptr;

		UI32 mBaseLayerIndex = 0;
		UI32 mLayerCount = 0;
		UI32 mBaseMipLevel = 0;
		UI32 mMipLevels = 0;
		ImageUsage mUsage = ImageUsage::Color;
	};
}