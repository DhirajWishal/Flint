// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Types\Handle.h"
#include "Core\Macros\Global.h"

namespace Flint
{
	namespace Interface
	{
		/**
		 * Instance handle.
		 * Instances are the backbone of a single backend API and is responsible for the creation of displays and devices.
		 */
		FLINT_DEFINE_HANDLE(InstanceHandle);

		/**
		 * Display handle.
		 * Displays contain information about a single display/ window and frames are rendered to it.
		 */
		FLINT_DEFINE_HANDLE(DisplayHandle);

		/**
		 * Device handle.
		 * This device handle represents a single or multiple GPUs and all the resources are bound to a device.
		 */
		FLINT_DEFINE_HANDLE(DeviceHandle);

		/**
		 * Render Target handle.
		 * Render targets perform rendering and are of two types.
		 * 1. Screen bound render target.
		 * 2. Off screen render target.
		 */
		FLINT_DEFINE_HANDLE(RenderTargetHandle);

		/**
		 * Process Node handle.
		 * A process node is a section in the render target which a frame can be processed (pre or post).
		 */
		FLINT_DEFINE_HANDLE(ProcessNodeHandle);

		/**
		 * Buffer handle.
		 * Buffers contain information about a block of data in the device.
		 */
		FLINT_DEFINE_HANDLE(BufferHandle);

		/**
		 * Image handle.
		 * Image handles contain information about a single image which may or may not have multiple image layers.
		 */
		FLINT_DEFINE_HANDLE(ImageHandle);

		/**
		 * Pipeline handle.
		 * Pipeline handles contain information about a single rendering pipeline. These are of 3 types.
		 * 1. Graphics pipeline.
		 * 2. Compute pipeline.
		 * 3. Ray Traced pipeline.
		 */
		FLINT_DEFINE_HANDLE(PipelineHandle);

		/**
		 * Command Buffer List handle.
		 * The command buffer list contains command which are to be submitted to the device to be executed.
		 */
		FLINT_DEFINE_HANDLE(CommandBufferListHandle);
	}
}