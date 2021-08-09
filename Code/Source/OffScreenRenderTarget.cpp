// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "OffScreenRenderTarget.hpp"

namespace Flint
{
	OffScreenRenderTarget::OffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList, OffScreenRenderTargetAttachment attachments, UI32 threadCount)
		: RenderTarget(pDevice, extent, bufferCount, pCommandBufferList, threadCount), mAttachments(attachments)
	{
		if (static_cast<UI8>(mAttachments) == 0)
			FLINT_THROW_INVALID_ARGUMENT("Off screen render target attachments should not be 0!");
	}
}