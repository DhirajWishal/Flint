// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ImageView.hpp"

namespace Flint
{
	ImageView::ImageView(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Image>& pImage, const uint32 baseLayerIndex, const uint32 layerCount, const uint32 baseMipLevel, const uint32 mipLevels, const ImageUsage usage)
		: DeviceBoundObject(pDevice), pParentImage(pImage), mBaseLayerIndex(baseLayerIndex), mLayerCount(layerCount), mBaseMipLevel(baseMipLevel), mMipLevels(mipLevels), mUsage(usage)
	{
		if (!pImage)
			throw std::runtime_error("The image pointer should not be nullptr!");
	}
}
