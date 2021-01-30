// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
#include "Core/Inputs/InputCenter.h"

namespace Flint
{
	namespace Interface
	{
		/**
		 * Create a new display object.
		 *
		 * @param instanceHandle: The instance handle to which the device is created to.
		 * @param width: The width of the display.
		 * @param height: The height of the display.
		 * @param pTitle: The title of the display.
		 * @return The display Handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(DisplayHandle, CreateDisplay, const InstanceHandle& instanceHandle, UI32 width, UI32 height, const char* pTitle);

		/**
		 * Destroy a created display.
		 *
		 * @param handle: The display handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyDisplay, const DisplayHandle& handle);

		/**
		 * Get the input center of a display object.
		 *
		 * @param handle: The display handle.
		 * @return The input center pointer.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(Inputs::InputCenter*, GetInputCenter, const DisplayHandle& handle);
	}
}