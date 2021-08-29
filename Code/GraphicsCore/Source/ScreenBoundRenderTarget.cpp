// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ScreenBoundRenderTarget.hpp"

namespace Flint
{
	ScreenBoundRenderTarget::ScreenBoundRenderTarget(const boost::shared_ptr<Device>& pDevice, const boost::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const boost::container::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode)
		: RenderTarget(pDevice, extent, bufferCount, imageAttachments), pDisplay(pDisplay)
	{
		if (!pDisplay)
			FLINT_THROW_INVALID_ARGUMENT("Screen bound render target device pointer should not be null!");
	}
}