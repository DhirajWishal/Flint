// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/MaterialProperties.hpp"
#include "GraphicsCore/Device.hpp"

namespace Flint
{
	namespace MaterialProperties
	{
		std::unique_ptr<Image> Flint::MaterialProperties::TextureData::CreateImage(Device* pDevice, const ImageType type, const ImageUsage usage, const uint32_t layers, const uint32_t mipLevels, const MultiSampleCount sampleCount)
		{
			if (mipLevels != 0)
				return pDevice->CreateImage(type, usage, mExtent, mPixelFormat, layers, mipLevels, pPixelData, sampleCount);
			else
				return pDevice->CreateImage(type, usage, mExtent, mPixelFormat, layers, Image::GetBestMipLevels(mExtent), pPixelData, sampleCount);
		}
		
		void TextureData::Clear()
		{
			free(pPixelData);
		}
	}
}