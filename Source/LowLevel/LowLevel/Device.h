// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.h"

namespace Flint
{
	namespace LowLevel
	{
		/**
		 * Device object.
		 * This object is responsible of creating, managing and destroying resources bound to a certain device.
		 */
		class Device {
		public:
			Device() {}
			~Device() {}

			/**
			 * Initialize the device.
			 *
			 * @param display: The display object which the device is bound to.
			 */
			void Initialize(const Display& display);

			/**
			 * Terminate the device.
			 */
			void Terminate();

		public:
			Interface::DeviceHandle GetHandle() const { return mHandle; }

		private:
			Interface::DeviceHandle mHandle = {};
		};
	}
}