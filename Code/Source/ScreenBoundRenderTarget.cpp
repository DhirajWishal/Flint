// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ScreenBoundRenderTarget.hpp"

namespace Flint
{
	ScreenBoundRenderTarget::ScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, SwapChainPresentMode presentMode)
		: RenderTarget(pDevice, extent, bufferCount, imageAttachments), pDisplay(pDisplay)
	{
		if (!pDisplay)
			FLINT_THROW_INVALID_ARGUMENT("Screen bound render target device pointer should not be null!");
	}
}