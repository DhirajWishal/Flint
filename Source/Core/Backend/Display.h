// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
#include "Core/Inputs/InputCenter.h"

namespace Flint
{
	namespace Backend
	{
		class Device;

		/**
		 * Display object.
		 * This object contains information about a single display object.
		 */
		class Display : public BackendObject {
		public:
			Display() {}

			/**
			 * Initialize the display.
			 *
			 * @param pInstance: The instance pointer.
			 * @param width: The width of the display.
			 * @param height: The height of the display.
			 * @param pTiele: The title of the display.
			 */
			virtual void Initialize(Instance* pInstance, UI32 width, UI32 height, const char* pTitle = "Flint engine") = 0;

			/**
			 * Update the display and poll inputs.
			 */
			virtual void Update() = 0;

			/**
			 * Terminate the display.
			 */
			virtual void Terminate() = 0;

			/**
			 * Create a new device object.
			 *
			 * @return The device pointer.
			 */
			virtual Device* CreatDevice() = 0;

		public:
			Instance* GetInstance() const { return pInstance; }
			Inputs::InputCenter* GetInputCenter() const { return const_cast<Inputs::InputCenter*>(&mInputCenter); }

		protected:
			Instance* pInstance = nullptr;
			Inputs::InputCenter mInputCenter = {};
		};
	}
}