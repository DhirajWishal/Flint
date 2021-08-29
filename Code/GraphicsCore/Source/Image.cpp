// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/Image.hpp"

namespace Flint
{	
	Image::Image(const boost::shared_ptr<Device>& pDevice, const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const UI8 layers, const UI32 mipLevels, const void* pImageData, const MultiSampleCount sampleCount)
		: DeviceBoundObject(pDevice), mType(type), mUsage(usage), mExtent(extent), mFormat(format), mLayerCount(layers), mMipLevels(mipLevels), mMultiSampleCount(sampleCount)
	{
		if (mExtent.IsZero())
			FLINT_THROW_INVALID_ARGUMENT("Image extent should not be 0!");

		if (mFormat == PixelFormat::UNDEFINED)
			FLINT_THROW_INVALID_ARGUMENT("Image pixel format should not be undefined!");

		if (!mLayerCount)
			FLINT_THROW_INVALID_ARGUMENT("Image layer count should be grater than 0!");

		if (!mMipLevels)
			FLINT_THROW_INVALID_ARGUMENT("Image mip levels must be grater than 0!");
	}
}