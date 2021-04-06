// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.h"
#include "Core\Backend\BackendObject.h"
#include "Core\Maths\Vector\Vector2.h"

namespace Flint
{
	class FDisplay;
	class FDevice;

	/**
	 * Flint Instance object.
	 * This object holds information about a single backend instance.
	 */
	class FInstance final : public FObject {
	public:
		FInstance();
		~FInstance();

		/**
		 * Initialize the instance object.
		 *
		 * @param enabelValidation: Whether or not to enable backend validation.
		 */
		void Initialize(bool enableValidation);

		/**
		 * Terminate the instance.
		 */
		void Terminate();

	public:
		/**
		 * Create a new display object.
		 * 
		 * @param extent: The display extent.
		 * @param pTitle: The display title.
		 * @return The newly created display object.
		 */
		FDisplay CreateDisplay(const Vector2 extent, const char* pTitle);

		/**
		 * Create a new device object.
		 * 
		 * @return The newly created device object.
		 */
		FDevice CreateDevice();
	};
}