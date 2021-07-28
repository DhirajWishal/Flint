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
		 * @param pCommandBufferList: The command buffer list pointer.
		 */
		ScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FExtent2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList)
			: RenderTarget(pDevice, extent, bufferCount, pCommandBufferList), pDisplay(pDisplay)
		{
			if (!pDisplay)
				FLINT_THROW_INVALID_ARGUMENT("Screen bound render target device pointer should not be null!");
		}

	public:
		/**
		 * Get the current frame index.
		 *
		 * @return The frame index.
		 */
		UI32 GetFrameIndex() const { return mFrameIndex; }

		/**
		 * Get the current image index.
		 *
		 * @return The image index.
		 */
		UI32 GetImageIndex() const { return mImageIndex; }

		/**
		 * Get the render targets clear screen color.
		 *
		 * @return The color container.
		 */
		virtual FColor4D GetClearColor() const = 0;

		/**
		 * Set the clear color values.
		 *
		 * @param newColor: The new color to set.
		 */
		virtual void SetClearColor(const FColor4D& newColor) = 0;

	protected:
		/**
		 * Increment the frame index.
		 * If the frame index is bigger than or equal to the buffer count, it is rolled back to 0.
		 */
		void IncrementFrameIndex() { mFrameIndex++; if (mFrameIndex >= mBufferCount) mFrameIndex = 0; }

		/**
		 * Recreate the render target.
		 * Recreating is essential when the frame buffer should be resized or if the swap chain is out of date.
		 */
		virtual void Recreate() = 0;

	protected:
		std::shared_ptr<Display> pDisplay = nullptr;

		UI32 mFrameIndex = 0;
		UI32 mImageIndex = 0;
	};
}