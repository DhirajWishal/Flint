// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ScreenBoundRenderTarget.hpp"

namespace Flint
{
	ScreenBoundRenderTarget::ScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor)
		: RenderTarget(pDevice, extent, bufferCount, imageAttachments), pDisplay(pDisplay)
	{
		if (!pDisplay)
			throw std::invalid_argument("Screen bound render target device pointer should not be null!");
	}
}