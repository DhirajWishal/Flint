// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
#include "Core/Inputs/InputCenter.h"

namespace Flint
{
	namespace LowLevel
	{
		/**
		 * Backend Display object.
		 */
		class Display {
		public:
			Display() {}
			~Display() {}

			/**
			 * Initialize the display.
			 * 
			 * @param instance: The instance which the display is created to.
			 * @param width: The width of the display.
			 * @param height: The height of the display.
			 * @param pTitle: The title of the display.
			 */
			void Initialize(const Instance& instance, UI32 width, UI32 height, const char* pTitle);

			/**
			 * Terminate the display.
			 */
			void Terminate();

		public:
			Inputs::InputCenter* GetInputCenter() const { return const_cast<Inputs::InputCenter*>(pInputCenter); }
			Interface::DisplayHandle GetHandle() const { return mHandle; }

		private:
			Interface::DisplayHandle mHandle = FLINT_NULL_HANDLE;
			Inputs::InputCenter* pInputCenter = nullptr;
		};
	}
}