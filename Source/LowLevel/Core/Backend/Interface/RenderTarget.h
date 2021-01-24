// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"

/**
 * The render target is the object which holds information about where the objects are rendered to. There are two
 * main types,
 * 1. Screen Bound (which renderes to a screen).
 * 2. Off Screen (which isnt rendered to a screen).
 *
 * Each of these 2 types have 3 sub-types,
 * 1. 2D.
 * 2. 3D.
 * 3. Ray Traced (hardware accelerated if supported or else the compute shader will be used).
 *
 * All the objects are assigned to a render target prior to execution/ render.
 */

namespace Flint
{
	namespace Interface
	{
		/**
		 * Render Target Extent structure.
		 */
		struct RenderTargetExtent {
			RenderTargetExtent() = default;
			RenderTargetExtent(UI32 width, UI32 height) : mWidth(width), mHeight(height) {}

			UI32 mWidth = 0;
			UI32 mHeight = 0;
		};

		/**
		 * Create a new screen bound 2D render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		inline RenderTargetHandleSB2D(*CreateRenderTargetSB2D)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Destroy a created screen bound 2D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetSB2D)(const RenderTargetHandleSB2D& handle) = nullptr;

		/**
		 * Create a new screen bound 3D render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		inline RenderTargetHandleSB3D(*CreateRenderTargetSB3D)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Destroy a created screen bound 3D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetSB3D)(const RenderTargetHandleSB3D& handle) = nullptr;

		/**
		 * Create a new ray traced render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		inline RenderTargetHandleSBRT(*CreateRenderTargetSBRT)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Destroy a created ray traced render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetSBRT)(const RenderTargetHandleSBRT& handle) = nullptr;

		/**
		 * Create a new off screen 2D render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		inline RenderTargetHandleOS2D(*CreateRenderTargetOS2D)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Destroy a created off screen 2D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetOS2D)(const RenderTargetHandleOS2D& handle) = nullptr;

		/**
		 * Create a new off screen 3D render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		inline RenderTargetHandleOS3D(*CreateRenderTargetOS3D)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Destroy a created off screen 3D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetOS3D)(const RenderTargetHandleOS3D& handle) = nullptr;

		/**
		 * Create a new ray traced render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		inline RenderTargetHandleOSRT(*CreateRenderTargetOSRT)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Destroy a created ray traced render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetOSRT)(const RenderTargetHandleOSRT& handle) = nullptr;
	}
}