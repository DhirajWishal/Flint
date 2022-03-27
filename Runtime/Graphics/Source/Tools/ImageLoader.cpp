// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Tools/ImageLoader.hpp"
#include "GraphicsCore/Device.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "Graphics/ThirdParty/stb_image.h"

namespace Flint
{
	ImageLoader::ImageLoader(const std::filesystem::path& assetFile)
	{
		int32_t texWidth = 0, texHeight = 0, texChannels = 0;

		if (stbi_is_hdr(assetFile.string().c_str()))
		{
			pPixelData = reinterpret_cast<unsigned char*>(stbi_loadf(assetFile.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha));
			mPixelFormat = PixelFormat::R32G32B32A32_SFLOAT;
		}
		else
		{
			pPixelData = stbi_load(assetFile.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			mPixelFormat = PixelFormat::R8G8B8A8_SRGB;
		}

		mExtent = FBox3D(texWidth, texHeight, 1);
	}

	ImageLoader::ImageLoader(const std::vector<std::filesystem::path>& assetFiles)
	{
		int32_t texWidth = 0, texHeight = 0, texChannels = 0;

		uint64_t offset = 0;
		for (uint64_t i = 0; i < assetFiles.size(); i++)
		{
			auto pixels = stbi_load(assetFiles[i].string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			uint64_t imageSize = static_cast<uint64_t>(texWidth) * static_cast<uint64_t>(texHeight) * 4;

			if (!imageSize)
				throw std::runtime_error("Couldn't load the requested image at location: " + assetFiles[i].string());

			if (!pPixelData)
				pPixelData = new unsigned char[imageSize * assetFiles.size()];

			std::copy(pixels, pixels + imageSize, pPixelData + offset);
			offset += imageSize;
		}

		mExtent = FBox3D(texWidth, texHeight, 1);
		mPixelFormat = PixelFormat::R8G8B8A8_SRGB;
	}

	ImageLoader::ImageLoader(ImageLoader&& other) : pPixelData(other.pPixelData), mExtent(other.mExtent), mPixelFormat(other.mPixelFormat)
	{
		other.pPixelData = nullptr;
	}

	ImageLoader::~ImageLoader()
	{
		if (pPixelData)
			stbi_image_free(pPixelData);
	}

	std::unique_ptr<Image> ImageLoader::CreateImage(Device* pDevice, const ImageType type, const ImageUsage usage, const uint32_t layers, const uint32_t mipLevels, const MultiSampleCount sampleCount) const
	{
		if (mipLevels != 0)
			return pDevice->CreateImage(type, usage, mExtent, mPixelFormat, layers, mipLevels, pPixelData, sampleCount);
		else
			return pDevice->CreateImage(type, usage, mExtent, mPixelFormat, layers, Image::GetBestMipLevels(mExtent), pPixelData, sampleCount);
	}

	MaterialProperties::TextureData ImageLoader::GetAsTextureData(const MaterialProperties::TextureType type)
	{
		auto data = MaterialProperties::TextureData(pPixelData, mExtent, mPixelFormat, type);
		pPixelData = nullptr;

		return data;
	}

	ImageLoader& ImageLoader::operator=(ImageLoader&& other)
	{
		pPixelData = other.pPixelData;
		mExtent = other.mExtent;
		mPixelFormat = other.mPixelFormat;

		other.pPixelData = nullptr;
		return *this;
	}
}