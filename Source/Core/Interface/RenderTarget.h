// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Display.h"

namespace Flint
{
	namespace Interface
	{
		/**
		 * Create a new off screen render target.
		 * Content from these render targets are not rendered to a screen/ display.
		 *
		 * @param deviceHandle: The device handle.
		 * @param extent: The extent of the buffer.
		 * @param bufferCount: The number of buffers in the render target. If set to 0, the minimum supported is set, if set to UI32_MAX, the maximum supported is set.
		 * @return The render target handle.
		 */
		inline FLINT_DEFINE_FUNCTION_POINTER(RenderTargetHandle, CreateOffScreenRenderTarget, DeviceHandle deviceHandle, const Vector2 extent, UI32 bufferCount);

		/**
		 * Create a new screen bound render target.
		 * Content from these render targets are rendered to a screen/ display.
		 *
		 * @param deviceHandle: The device handle.
		 * @param displayHandle: The display handle to which the data are rendered to.
		 * @param extent: The extent of the buffer.
		 * @param bufferCount: The number of buffers in the render target. If set to 0, the minimum supported is set, if set to UI32_MAX, the maximum supported is set.
		 * @return The render target handle.
		 */
		inline FLINT_DEFINE_FUNCTION_POINTER(RenderTargetHandle, CreateScreenBoundRenderTarget, DeviceHandle deviceHandle, DisplayHandle displayHandle, const Vector2 extent, UI32 bufferCount);

		/**
		 * Destroy a created render target.
		 *
		 * @param handle: The render target to be destroyed.
		 */
		inline FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRenderTarger, RenderTargetHandle handle);
	}
}