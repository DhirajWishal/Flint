// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"
#include "Display.hpp"

namespace Flint
{
	/**
	 * Flint screen bound render target.
	 * Screen bound render targets renders frames to a display object. This display object must be compatible with a device object.
	 */
	class FLINT_API ScreenBoundRenderTarget : public RenderTarget
	{
	public:
		ScreenBoundRenderTarget(Device& device, Display& display, const FExtent2D& extent, const UI32 bufferCount, CommandBufferList& commandBufferList)
			: RenderTarget(device, extent, bufferCount, commandBufferList), mDevice(device) {}

	protected:
		Device& mDevice;
	};
}