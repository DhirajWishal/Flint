// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "OffScreenRenderTarget.hpp"

namespace Flint
{
	OffScreenRenderTarget::OffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, OffScreenRenderTargetType type, const FBox2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList, UI32 threadCount)
		: RenderTarget(pDevice, extent, bufferCount, pCommandBufferList, threadCount), mType(type)
	{
	}
}