// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/MaterialProperties.hpp"
#include "GraphicsCore/Device.hpp"

namespace Flint
{
	namespace MaterialProperties
	{
		std::unique_ptr<Image> Flint::MaterialProperties::TextureData::CreateImage(Device* m_pDevice, const ImageType type, const ImageUsage usage, const uint32_t layers, const uint32_t mipLevels, const MultiSampleCount sampleCount)
		{
			if (mipLevels != 0)
				return m_pDevice->CreateImage(type, usage, m_Extent, m_PixelFormat, layers, mipLevels, pPixelData, sampleCount);
			else
				return m_pDevice->CreateImage(type, usage, m_Extent, m_PixelFormat, layers, Image::GetBestMipLevels(m_Extent), pPixelData, sampleCount);
		}
		
		void TextureData::Clear()
		{
			free(pPixelData);
		}
	}
}