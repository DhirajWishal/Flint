// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Image.hpp"

namespace Flint
{
	Image::Image(const std::shared_ptr<Device>& pDevice, ImageType type, ImageUsage usage, const FBox3D& extent, PixelFormat format, UI8 layers, UI32 mipLevels, const void* pImageData)
		: DeviceBoundObject(pDevice), mType(type), mUsage(usage), mExtent(extent), mFormat(format), mLayerCount(layers), mMipLevels(mipLevels)
	{
		if (mExtent.mWidth == 0 || mExtent.mHeight == 0 || mExtent.mDepth == 0)
			FLINT_THROW_INVALID_ARGUMENT("Image extent should not be 0!");

		if (mFormat == PixelFormat::UNDEFINED)
			FLINT_THROW_INVALID_ARGUMENT("Image pixel format should not be undefined!");

		if (!mLayerCount)
			FLINT_THROW_INVALID_ARGUMENT("Image layer count should be grater than 0!");

		if (!mMipLevels)
			FLINT_THROW_INVALID_ARGUMENT("Image mip levels must be grater than 0!");

		if (!pImageData)
			FLINT_THROW_INVALID_ARGUMENT("Provided image data pointer should not be null!");
	}
}