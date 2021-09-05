// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/ImageLoader.hpp"
#include "GraphicsCore/Device.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "Engine/ThirdParty/stb_image.h"

namespace Flint
{
	ImageLoader::ImageLoader(const std::filesystem::path& assetFile)
	{
		I32 texWidth = 0, texHeight = 0, texChannels = 0;
		pPixelData = stbi_load(assetFile.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		mExtent = FBox3D(texWidth, texHeight, 1);
		mPixelFormat = PixelFormat::R8G8B8A8_SRGB;
	}
	
	ImageLoader::~ImageLoader()
	{
		stbi_image_free(pPixelData);
	}
}