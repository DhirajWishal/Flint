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
		 * 
		 * @return Returns true if successfully presented, returns false if a recreation is needed.
		 */
		virtual bool PresentToDisplay() = 0;

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

		/**
		 * Get the current image index.
		 *
		 * @return The image index.
		 */
		virtual const UI32 GetImageIndex() const = 0;

		/**
		 * Check if the resources should be recreated.
		 * This could happen as a result of a resize. After recreating all the resources, make sure to notify the render target using NotifyRecreated().
		 * 
		 * @return The boolean value.
		 */
		const bool ShouldRecreateResources() const { return bShouldRecreateResources; }

		/**
		 * Notify after recreating the resources.
		 */
		void NotifyRecreated() { bShouldRecreateResources = false; }

	protected:
		std::shared_ptr<Display> pDisplay = nullptr;

		bool bShouldRecreateResources = false;
	};
}