// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

#include "Core\Inputs\InputCenter.h"
#include "Core\Maths\Vector\Vector2.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Display object.
		 * This object contains information about a single display instance. Screen bound render targets use these objects to render data to the screen.
		 * Displays also contains an input center where input data are stored.
		 */
		class Display : public BackendObject
		{
		public:
			/**
			 * Create the display using the basic information.
			 *
			 * @param pInstance: The instance pointer.
			 * @param extent: The extent of the display.
			 * @param pTitle: The title of the display.
			 */
			Display(Instance* pInstance, const Vector2 extent, const char* pTitle) : pInstance(pInstance), mExtent(extent), pTitle(pTitle) {}

			/**
			 * Update the display.
			 * This method polls inputs and refreshes the display.
			 */
			virtual void Update() = 0;

			/**
			 * Get the input center of the display.
			 *
			 * @return The input center pointer.
			 */
			Inputs::InputCenter* GetInputCenter() const { return const_cast<Inputs::InputCenter*>(&mInputsCenter); }

			/**
			 * Get the current extent of the display.
			 *
			 * @return the extent of the display.
			 */
			Vector2 GetExtent() const { return mExtent; }

			/**
			 * Get the title of the display object.
			 *
			 * @return The title.
			 */
			const char* GetTitle() const { return pTitle; }

			/**
			 * Get the instance pointer of the display.
			 *
			 * @return The current instance pointer.
			 */
			Instance* GetInstance() const { return pInstance; }

		protected:
			Inputs::InputCenter mInputsCenter = {};
			Instance* pInstance = nullptr;

			Vector2 mExtent = Vector2::Zero;
			const char* pTitle = nullptr;
		};
	}
}