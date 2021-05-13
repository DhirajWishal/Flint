// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Device bound object.
		 * This object is the base class for all the device bound objects in Flint.
		 */
		class DeviceBoundObject : public BackendObject
		{
		public:
			/**
			 * Construct the device bound object.
			 *
			 * @param pDevice: The device object pointer.
			 */
			DeviceBoundObject(Device* pDevice) : pDevice(pDevice) {}

			/**
			 * Get the device object pointer.
			 *
			 * @return The device pointer.
			 */
			Device* GetDevice() { return pDevice; }

			/**
			 * Get the device object pointer.
			 *
			 * @return The device pointer.
			 */
			const Device* GetDevice() const { return pDevice; }

		protected:
			Device* pDevice = nullptr;
		};
	}
}