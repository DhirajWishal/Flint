// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/MaterialProperties.hpp"
#include "GraphicsCore/Device.hpp"

namespace Flint
{
	namespace MaterialProperties
	{
		std::shared_ptr<Image> Flint::MaterialProperties::TextureData::CreateImage(const std::shared_ptr<Device>& pDevice, const ImageType type, const ImageUsage usage, const UI32 layers, const UI32 mipLevels, const MultiSampleCount sampleCount)
		{
			if (mipLevels != 0)
				return pDevice->CreateImage(type, usage, mExtent, mPixelFormat, layers, mipLevels, pPixelData, sampleCount);
			else
				return pDevice->CreateImage(type, usage, mExtent, mPixelFormat, layers, Image::GetBestMipLevels(mExtent), pPixelData, sampleCount);
		}
	}
}