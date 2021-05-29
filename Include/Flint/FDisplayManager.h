// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataTypes.h"
#include "Core/Interface.h"

namespace Flint
{
	DEFINE_HANDLE_UI8(FDisplayHandle);

	/**
	 * Flint display manager object.
	 * This object handles all the display based services.
	 */
	class FLINT_API FDisplayManager
	{
	public:
		FDisplayManager() = default;

		/**
		 * Create a new display object.
		 *
		 * @param width: Width of the display.
		 * @param height: The height of the display.
		 * @param title: The title of the display.
		 * @return The created display handle.
		 */
		virtual FDisplayHandle CreateDisplay(const UI32 width, const UI32 height, const std::string& title) = 0;

		/**
		 * Update the display handle.
		 * 
		 * @param handle: The display handle.
		 */
		virtual void UpdateDisplay(const FDisplayHandle& handle) = 0;

		/**
		 * Destroy a created display handle.
		 *
		 * @param handle: The display handle.
		 */
		virtual void DestroyDisplay(FDisplayHandle& handle) = 0;
	};
}