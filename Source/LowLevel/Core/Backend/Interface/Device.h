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
		inline DeviceHandle(*CreateDevice)(const DisplayHandle& displayHandle) = nullptr;

		/**
		 * Destroy a created device.
		 *
		 * @param handle: The device handle to be destroyed.
		 */
		inline void (*DestroyDevice)(const DeviceHandle& handle) = nullptr;
	}
}