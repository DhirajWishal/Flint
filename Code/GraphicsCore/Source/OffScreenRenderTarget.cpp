// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/OffScreenRenderTarget.hpp"

namespace Flint
{	
	OffScreenRenderTarget::OffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const uint32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments)
		: RenderTarget(pDevice, extent, bufferCount, imageAttachments)
	{
	}
}