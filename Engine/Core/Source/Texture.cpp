// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Texture.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{

	Texture::Texture(const std::shared_ptr<Device>& pDevice, uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format, uint32_t mipLevels /*= 0*/, Multisample multisampleCount /*= Multisample::One*/)
		: DeviceBoundObject(pDevice)
		, m_Width(width)
		, m_Height(height)
		, m_Usage(usage)
		, m_Format(format)
		, m_MipLevels(mipLevels == 0 ? static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1 : mipLevels)
		, m_MultisampleCount(multisampleCount)
	{
		if (width == 0)
			throw InvalidArgumentError("The texture's width should be grater than 0!");

		if (height == 0)
			throw InvalidArgumentError("The texture's height should be grater than 0!");

		if (format == PixelFormat::Undefined)
			throw InvalidArgumentError("The texture's pixel format should not be undefined!");
	}

}