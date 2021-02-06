// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.h"

namespace Flint
{
	namespace Backend
	{
		enum class MemoryProfile : UI8 {
			TRANSFER_FRIENDLY,
			DEVICE_ONLY,
		};

		/**
		 * Device object.
		 * This represents a single device object (GPU) and is responsible of creating buffers, render targets, etc...
		 */
		class Device : public BackendObject {
		public:
			Device() {}

			/**
			 * Initialize the device.
			 *
			 * @param pDisplay: The deisplay which the object is bound to.
			 */
			virtual void Initialize(Display* pDisplay) = 0;

			/**
			 * Terminate the device.
			 */
			virtual void Terminate() = 0;

		public:
			Display* GetDisplay() const { return pDisplay; }

		protected:
			Display* pDisplay = nullptr;
		};
	}
}