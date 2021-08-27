// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "OffScreenRenderTarget.hpp"
#include "Display.hpp"
#include "SwapChain.hpp"

namespace Flint
{
	/**
	 * Flint screen bound render target.
	 * Screen bound render targets renders frames to a display object. This display object must be compatible with a device object.
	 */
	class ScreenBoundRenderTarget : public RenderTarget
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 * @param pDisplay: The display pointer.
		 * @param extent: The screen bound render target extent.
		 * @param bufferCount: The frame buffer count.
		 * @param presentMode: The image present mode to the display.
		 */
		ScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode);

		/**
		 * Present the images to the display.
		 */
		virtual void PresentToDisplay() = 0;

		/**
		 * Recreate the screen bound render target.
		 */
		virtual void Recreate() = 0;

	public:
		/**
		 * Get the display pointer.
		 * 
		 * @return The display pointer.
		 */
		const std::shared_ptr<Display> GetDisplay() const { return pDisplay; }

	protected:
		std::shared_ptr<Display> pDisplay = nullptr;
	};
}