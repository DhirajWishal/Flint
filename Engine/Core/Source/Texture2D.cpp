// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Texture2D.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"
#include "Core/Errors/AssetError.hpp"

#include <stb_image.h>

namespace Flint
{
	Texture2D::Texture2D(const std::shared_ptr<Device>& pDevice, uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format, uint32_t mipLevels /*= 0*/, Multisample multisampleCount /*= Multisample::One*/, const std::byte* pDataStore /*= nullptr*/)
		: Texture(pDevice, width, height, usage, format, mipLevels, multisampleCount)
	{
	}

	std::shared_ptr<Flint::Texture2D> Texture2D::LoadFromFile(const std::shared_ptr<Device>& pDevice, const std::filesystem::path& assetFile, ImageUsage usage, Multisample multisampleCount /*= Multisample::One*/)
	{
		// Load the image.
		int32_t width, height, channels;
		const auto pPixelData = reinterpret_cast<std::byte*>(stbi_load(assetFile.string().c_str(), &width, &height, &channels, STBI_rgb_alpha));

		// Validate the image.
		if (!pPixelData)
			throw AssetError("Failed to load the image file!");

		// Create the texture and clear up the memory.
		auto pTexture = pDevice->createTexture2D(width, height, usage, PixelFormat::R8G8B8A8_SRGB, 0, multisampleCount, pPixelData);
		stbi_image_free(pPixelData);

		return pTexture;
	}
}