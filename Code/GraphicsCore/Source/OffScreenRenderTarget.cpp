// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/OffScreenRenderTarget.hpp"

namespace Flint
{	
	OffScreenRenderTarget::OffScreenRenderTarget(const boost::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const boost::container::vector<RenderTargetAttachment>& imageAttachments)
		: RenderTarget(pDevice, extent, bufferCount, imageAttachments)
	{
	}
}