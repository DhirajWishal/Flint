// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
#include "Core\Maths\Vector\Vector2.h"
#include "Core\Inputs\InputCenter.h"

namespace Flint
{
	namespace Interface
	{
		namespace Template
		{
			class Display
			{
			public:
				Display(Inputs::InputCenter* pInputCenter) : pInputCenter(pInputCenter) {}
				virtual ~Display() {}

				Inputs::InputCenter* GetInputCenter() { return pInputCenter; }
				const Inputs::InputCenter* GetInputCenter() const { return pInputCenter; }

			protected:
				Inputs::InputCenter* pInputCenter = nullptr;
			};
		}

		/**
		 * Create a new display.
		 *
		 * @param instanceHandle: The instance which the display is bound to.
		 * @param extent: The extent of the display.
		 * @param pTitle: The title of the display.
		 * @return The display handle.
		 */
		inline FLINT_DEFINE_FUNCTION_POINTER(DisplayHandle, CreateDisplay, InstanceHandle instanceHandle, const Vector2 extent, const char* pTitle, Inputs::InputCenter* pInputCenter);

		/**
		 * Update the display handle.
		 * This will poll the actual inputs.
		 * 
		 * @param handle: The display handle to be updated.
		 */
		inline FLINT_DEFINE_FUNCTION_POINTER(void, UpdateDisplay, DisplayHandle handle);

		/**
		 * Destroy a created display handle.
		 *
		 * @param handle: The handle to be destroyed.
		 */
		inline FLINT_DEFINE_FUNCTION_POINTER(void, DestroyDisplay, DisplayHandle handle);
	}
}