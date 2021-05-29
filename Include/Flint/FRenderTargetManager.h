// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDeviceManager.h"

namespace Flint
{
	DEFINE_HANDLE_UI16(FRenderTargetHandle);

	/**
	 * Flint render target manager object.
	 * This object manages all the aspects of render targets.
	 *
	 * There are 2 main types of render targets.
	 * 1. Screen bound render target.
	 * 2. Off screen render targets.
	 *
	 * Screen bound render targets use a display handle and renders frames to the display.
	 * Off screen render targets does not render anything to a display, but is used for processing data.
	 */
	class FLINT_API FRenderTargetManager
	{
	public:
		FRenderTargetManager() = default;

		/**
		 * Create a new screen bound render target.
		 *
		 * @param deviceHandle: The device handle.
		 * @param displayHandle: The display handle.
		 * @param extent: The render target extent.
		 * @param bufferCount: The number of buffers in the frame buffer. If set to 0, the default amount is set. If set to UI32_MAX, the maximum supported is set.
		 * @return The created render target handle.
		 */
		virtual FRenderTargetHandle CreateScreenBoundRenderTarget(const FDeviceHandle& deviceHandle, const FDisplayHandle& displayHandle, const FExtent2D& extent, UI32 bufferCount = 0) = 0;

		/**
		 * Destroy a created render target.
		 *
		 * @param handle: The render target handle.
		 */
		virtual void DestroyRenderTarget(FRenderTargetHandle& handle) = 0;
	};
}