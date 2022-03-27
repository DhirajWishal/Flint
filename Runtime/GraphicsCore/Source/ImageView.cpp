// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ImageView.hpp"

namespace Flint
{
	ImageView::ImageView(Device* pDevice, const Image* pImage, const uint32_t baseLayerIndex, const uint32_t layerCount, const uint32_t baseMipLevel, const uint32_t mipLevels, const ImageUsage usage)
		: DeviceBoundObject(pDevice), pParentImage(pImage), mBaseLayerIndex(baseLayerIndex), mLayerCount(layerCount), mBaseMipLevel(baseMipLevel), mMipLevels(mipLevels), mUsage(usage)
	{
		if (!pImage)
			throw std::runtime_error("The image pointer should not be nullptr!");
	}
}
