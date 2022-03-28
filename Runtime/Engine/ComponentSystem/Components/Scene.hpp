// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore.hpp"

namespace Flint
{
	namespace Components
	{
		/**
		 * Screen bound scene component.
		 * These scenes can render data to the display.
		 */
		struct ScreenBoundScene
		{
			/**
			 * Construct the component.
			 * 
			 * @param pGraphicsDevice The graphics device pointer.
			 * @param extent The extent of the display.
			 * @param title The title of the display.
			 */
			ScreenBoundScene(GraphicsDevice* pGraphicsDevice, const FBox2D extent, const std::string& title);

			AbstractionObject m_Display = {};
			AbstractionObject m_RenderTarget = {};
		};
	}
}
