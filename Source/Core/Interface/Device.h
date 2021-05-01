// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

namespace Flint
{
	namespace Interface
	{
		namespace Template
		{
			class Device
			{
			public:
				Device() {}
				virtual ~Device() {}
			};
		}

		/**
		 * Create a new device handle.
		 *
		 * @param instanceHandle: The instance to which the device is bound to.
		 * @return The device handle.
		 */
		inline FLINT_DEFINE_FUNCTION_POINTER(DeviceHandle, CreateDevice, InstanceHandle instanceHandle);

		/**
		 * Destroy a created device.
		 *
		 * @param handle: The handle to be destroyed.
		 */
		inline FLINT_DEFINE_FUNCTION_POINTER(void, DestroyDevice, DeviceHandle handle);
	}
}