// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"
#include "Display.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint screen bound render target.
		 * Screen bound render targets renders frames to a display object. This display object must be compatible with a device object.
		 */
		class ScreenBoundRenderTarget : public RenderTarget
		{
		public:
			ScreenBoundRenderTarget(Device& device, Display& display, const FExtent2D& extent, const UI32 bufferCount, CommandBufferList& commandBufferList)
				: RenderTarget(device, extent, bufferCount, commandBufferList), mDisplay(display) {}

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
			Display& mDisplay;

			UI32 mFrameIndex = 0;
			UI32 mImageIndex = 0;
		};
	}
}