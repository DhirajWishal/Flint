// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Image.hpp"

namespace Flint
{
	/**
	 * Flint image view object.
	 * Image views are used to bind images to the resource packages.
	 */
	template<class DeviceT, class ImageT>
	class ImageView : public DeviceBoundObject<DeviceT>
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
		ImageView(DeviceT* pDevice, const ImageT* pImage, const uint32_t baseLayerIndex, const uint32_t layerCount, const uint32_t baseMipLevel, const uint32_t mipLevels, const ImageUsage usage)
			: DeviceBoundObject(pDevice), pParentImage(pImage), mBaseLayerIndex(baseLayerIndex), mLayerCount(layerCount), mBaseMipLevel(baseMipLevel), mMipLevels(mipLevels), mUsage(usage)
		{
			if (!pImage)
				throw std::runtime_error("The image pointer should not be nullptr!");
		}

	public:
		/**
		 * Get the parent image pointer.
		 *
		 * @return The image pointer.
		 */
		const ImageT* GetParentImage() const { return pParentImage; }

		/**
		 * Get the base layer index.
		 *
		 * @return The layer index.
		 */
		uint32_t GetBaseLayerIndex() const { return mBaseLayerIndex; }

		/**
		 * Get the layer count.
		 *
		 * @return The layer count.
		 */
		uint32_t GetLayerCount() const { return mLayerCount; }

		/**
		 * Get the base mip level.
		 *
		 * @return The mip level
		 */
		uint32_t GetBaseMipLevel() const { return mBaseMipLevel; }

		/**
		 * Get the mip levels.
		 *
		 * @return The mip levels.
		 */
		uint32_t GetMipLevels() const { return mMipLevels; }

	protected:
		const ImageT* pParentImage = nullptr;

		uint32_t mBaseLayerIndex = 0;
		uint32_t mLayerCount = 0;
		uint32_t mBaseMipLevel = 0;
		uint32_t mMipLevels = 0;
		ImageUsage mUsage = ImageUsage::Color;
	};
}