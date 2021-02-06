// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendObject.h"

namespace Flint
{
	namespace Backend
	{
		class Display;

		/**
		 * Instance object.
		 * This is the main backend instance object and is responsible of creating the device object.
		 */
		class Instance : public BackendObject {
		public:
			Instance() {}

			/**
			 * Initialize the instance.
			 *
			 * @param enableValidation: Whether or not to enable backend validation.
			 */
			virtual void Initialize(bool enableValidation) = 0;

			/**
			 * Terminate the backend instance.
			 */
			virtual void Terminate() = 0;

		public:
			/**
			 * Create a new display object.
			 *
			 * @param width: The width of the display.
			 * @param height: The height of the display.
			 * @param pTitle: The title of the display.
			 * @return The display object pointer.
			 */
			virtual Display* CreateDisplay(UI32 width, UI32 height, const char* pTitle) = 0;

		protected:
			bool mEnableValidation = true;
		};
	}
}