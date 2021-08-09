// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "OffScreenRenderTarget.hpp"
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
		 * @param threadCount: The number of threads to use. Default is 0.
		 */
		ScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList, UI32 threadCount = 0);

		/**
		 * Prepare the static resources to draw.
		 * This will bind all of the current content to the command buffer to be drawn.
		 */
		virtual void PrepareStaticResources() = 0;

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
		const UI32 GetFrameIndex() const { return mFrameIndex; }

		/**
		 * Get the current image index.
		 *
		 * @return The image index.
		 */
		const UI32 GetImageIndex() const { return mImageIndex; }

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

	public:
		/**
		 * Attach an off screen render target to the screen bound render target.
		 * These render targets will be executed right before the draw instances.
		 *
		 * @param pOffScreenRenderTarget: The off screen render target pointer.
		 */
		void AttachOffScreenRenderTarget(const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget);

		/**
		 * Detach an off screen render target from the screen bound render target.
		 * 
		 * @param pOffScreenRenderTarget: The render target pointer.
		 */
		void DetachOffScreenRenderTarget(const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget);

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
		std::vector<std::shared_ptr<OffScreenRenderTarget>> pOffScreenRenderTargets;

		UI32 mFrameIndex = 0;
		UI32 mImageIndex = 0;
	};
}