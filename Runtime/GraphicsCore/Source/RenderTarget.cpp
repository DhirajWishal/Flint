// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/RenderTarget.hpp"
#include "GraphicsCore/GraphicsPipeline.hpp"

namespace Flint
{
	RenderTarget::RenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments)
		: DeviceBoundObject(pDevice), mExtent(extent), mBufferCount(bufferCount), mAttachments(imageAttachments)
	{
		if (extent.IsZero())
			throw std::invalid_argument("Render target width and height should be greater than 0!");

		if (bufferCount == 0)
			throw std::invalid_argument("Render target buffer count should be greater than 0!");
	}
}