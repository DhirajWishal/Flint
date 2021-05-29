// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDisplayManager.h"

namespace Flint
{
	DEFINE_HANDLE_UI8(FDeviceHandle);

	/**
	 * Flint device manager object.
	 * This object is responsible for all the device based operations.
	 */
	class FLINT_API FDeviceManager
	{
	public:
		FDeviceManager() = default;

		/**
		 * Create a new device handle.
		 * 
		 * @return The created display handle.
		 */
		virtual FDeviceHandle CreateDevice() = 0;

		/**
		 * Destroy a created device.
		 * 
		 * @param handle: The handle to be terminated.
		 */
		virtual void DestroyDevice(FDeviceHandle& handle) = 0;

		/**
		 * Check if a device and display is compatible.
		 * This is necessary for screen bound render targets.
		 * 
		 * @param deviceHandle: The device handle to check with.
		 * @param displayHandle: The display handle to check.
		 * @return Boolean value stating if compatible or not (true or false respectively).
		 */
		virtual bool CheckDeviceAndDisplayCompatibility(const FDeviceHandle& deviceHandle, const FDisplayHandle& displayHandle) = 0;
	};
}