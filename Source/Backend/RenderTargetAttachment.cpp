// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Backend/RenderTargetAttachment.hpp"
#include "Flint/Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{
	namespace Backend
	{
		RenderTargetAttachment::RenderTargetAttachment(const std::shared_ptr<Device>& pDevice, uint32_t width, uint32_t height, AttachmentType type, PixelFormat format, Multisample multisample)
			: DeviceBoundObject(pDevice), m_Width(width), m_Height(height), m_Type(type), m_Format(format), m_Multisample(multisample)
		{
			if (width == 0)
				throw InvalidArgumentError("The render target attachment's width should be grater than 0!");

			if (height == 0)
				throw InvalidArgumentError("The render target attachment's height should be grater than 0!");

			if (format == PixelFormat::Undefined)
				throw InvalidArgumentError("The render target attachment's pixel format should not be undefined!");
		}
	}
}