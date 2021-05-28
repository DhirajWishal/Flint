// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

namespace Flint
{
	class FDisplayManager;

	/**
	 * Flint display handle struct.
	 */
	struct FDisplayHandle
	{
		FDisplayHandle() = default;
		FDisplayHandle(FDisplayManager* pManager, UI8 index) : pManager(pManager), mIndex(index) {}

		void Update() { pManager->UpdateDisplay(*this); }

		operator const UI8() const { return mIndex; }

	private:
		FDisplayManager* pManager = nullptr;
		UI8 mIndex = 0;
	};

	/**
	 * Flint display manager object.
	 * This object handles all the display based services.
	 */
	class FDisplayManager
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
		 * Terminate a created display handle.
		 *
		 * @param handle: The display handle.
		 */
		virtual void Terminate(FDisplayHandle& handle) = 0;
	};
}