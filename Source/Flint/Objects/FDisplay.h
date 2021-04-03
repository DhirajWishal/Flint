// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FInstance.h"
#include "Core\Inputs\InputCenter.h"

namespace Flint
{
	constexpr const UI64 GetDisplaySize();
	constexpr const UI64 GetDisplayAlignment();

	/**
	 * Flint device object.
	 * This object holds information about a single backend display object.
	 */
	class FDisplay final : public FObject<GetDisplaySize(), GetDisplayAlignment()> {
	public:
		FDisplay();
		~FDisplay();

		/**
		 * Initialize the display object.
		 *
		 * @param instance: The instance object.
		 * @param extent: The window extent.
		 * @param pTitle: The window title.
		 */
		void Initialize(const FInstance& instance, const Vector2 extent, const char* pTitle);

		/**
		 * Terminate the display.
		 */
		void Terminate();

		/**
		 * Update the display.
		 * This method polls all the inputs.
		 */
		void Update();

		/**
		 * Get input center.
		 *
		 * @return The input center pointer.
		 */
		Inputs::InputCenter* GetInputCenter() const;

		/**
		 * Get the display extent.
		 * 
		 * @return The extent.
		 */
		Vector2 GetExtent() const;
	};
}