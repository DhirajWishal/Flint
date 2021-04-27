// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.h"
#include "Pipeline.h"

namespace Flint
{
	/**
	 * Flint screen bound render target object.
	 * This object is used to draw data to a display object. Make sure that the display and device are compatible.
	 */
	class FScreenBoundRenderTarget : public FRenderTarget {
	public:
		FScreenBoundRenderTarget() {}
		virtual ~FScreenBoundRenderTarget() {}

		/**
		 * Initialize the render target.
		 *
		 * @param pDevice: The device pointer.
		 * @param pDisplay: The display pointer.
		 * @param bufferCount: The number of frame buffers the render target contains.
		 */
		virtual void Initialize(FDevice* pDevice, FDisplay* pDisplay, UI64 bufferCount) = 0;

		/**
		 * Get the current frame index.
		 *
		 * @return The index.
		 */
		UI32 GetFrameIndex() const { return mFrameIndex; }

		/**
		 * Get the current image index.
		 *
		 * @return The index.
		 */
		UI32 GetImageIndex() const { return mImageIndex; }

	protected:
		void IncrementIndex() { mFrameIndex++; if (mFrameIndex >= mBufferCount) mFrameIndex = 0; }

		FDisplay* pDisplay = nullptr;

		UI32 mFrameIndex = 0;
		UI32 mImageIndex = 0;
	};
}