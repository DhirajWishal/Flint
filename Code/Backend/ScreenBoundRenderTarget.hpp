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

		/**
		 * Begin a new frame.
		 * This call prepares resources for a new draw call and must be called prior to updating resources.
		 */
		virtual void BeginFrame() = 0;

		/**
		 * Update the resources.
		 * This call updates draw resources.
		 */
		virtual void Update() = 0;

		/**
		 * Submit the frame to the device.
		 * This call draws data to the screen by submitting the draw call to the device.
		 */
		virtual void SubmitFrame() = 0;

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