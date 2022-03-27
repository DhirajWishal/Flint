// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/Image.hpp"

#include <cmath>

namespace Flint
{	
	Image::Image(Device* pDevice, const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const uint8_t layers, const uint32_t mipLevels, const void* pImageData, const MultiSampleCount sampleCount)
		: DeviceBoundObject(pDevice), mType(type), mUsage(usage), mExtent(extent), mFormat(format), mLayerCount(layers), mMipLevels(mipLevels), mMultiSampleCount(sampleCount)
	{
		if (mExtent.IsZero())
			throw std::invalid_argument("Image extent should not be 0!");

		if (mFormat == PixelFormat::Undefined)
			throw std::invalid_argument("Image pixel format should not be undefined!");

		if (!mLayerCount)
			throw std::invalid_argument("Image layer count should be grater than 0!");

		if (!mMipLevels)
			throw std::invalid_argument("Image mip levels must be grater than 0!");
	}
	
	uint32_t Image::GetBestMipLevels(const FBox3D extent)
	{
		return static_cast<uint32_t>(std::floor(std::log2(std::max(extent.mWidth, extent.mHeight))) + 1);
	}
}