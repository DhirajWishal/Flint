// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
#include "Core\Maths\Vector\Vector2.h"
#include "Core\Inputs\InputCenter.h"

namespace Flint
{
	/**
	 * Flint display object.
	 * This object is the base class for all the backend display objects. Displays are responsible of creating a window on the screen to where content can be drawn to.
	 */
	class FDisplay : public BackendObject {
	public:
		FDisplay() {}
		virtual ~FDisplay() {}

		/**
		 * Initialize the display.
		 *
		 * @param pInstance: The instance pointer.
		 * @param extent: The display extent.
		 * @param pTitle: The display title.
		 */
		virtual void Initialize(FInstance* pInstance, const Vector2 extent, const char* pTitle) = 0;

		/**
		 * Update the display.
		 * This also polls inputs.
		 */
		virtual void Update() = 0;

		/**
		 * Terminate the display.
		 */
		virtual void Terminate() = 0;

		/**
		 * Get the input center of this display.
		 *
		 * @return The input center pointer.
		 */
		Inputs::InputCenter* GetInputCenter() const { return const_cast<Inputs::InputCenter*>(&mInputsCenter); }

		/**
		 * Get the display extent.
		 *
		 * @return The extent.
		 */
		Vector2 GetExtent() const { return mExtent; }

		/**
		 * Get the display title.
		 *
		 * @return The title as const char*.
		 */
		const char* GetTitle() const { return pTitle; }

		/**
		 * Get the instance which the display is bound to.
		 *
		 * @return The instance pointer.
		 */
		FInstance* GetInstance() const { return pInstance; }

	protected:
		Inputs::InputCenter mInputsCenter = {};
		FInstance* pInstance = nullptr;

		Vector2 mExtent = Vector2::Zero;
		const char* pTitle = nullptr;
	};
}