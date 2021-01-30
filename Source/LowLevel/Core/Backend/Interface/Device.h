// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.h"

namespace Flint
{
	namespace Interface
	{
		/**
		 * Create a new device capable of graphics and compute.
		 *
		 * @param displayHandle: The display handle which the device is bound to.
		 * @return The device handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(DeviceHandle, CreateDevice, const DisplayHandle& displayHandle);

		/**
		 * Destroy a created device.
		 *
		 * @param handle: The device handle to be destroyed.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyDevice, const DeviceHandle& handle);
	}
}