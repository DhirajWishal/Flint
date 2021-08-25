// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "RenderTarget.hpp"
#include "GraphicsPipeline.hpp"

namespace Flint
{
	RenderTarget::RenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments)
		: DeviceBoundObject(pDevice), mExtent(extent), mBufferCount(bufferCount), mAttachments(imageAttachments)
	{
		if (extent.IsZero())
			FLINT_THROW_INVALID_ARGUMENT("Render target width and height should be greater than 0!");

		if (bufferCount == 0)
			FLINT_THROW_INVALID_ARGUMENT("Render target buffer count should be greater than 0!");
	}
}