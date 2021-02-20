// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace Flint
{
	/**
	 * Draw call manager object.
	 * This object manages all the draw calls carried out by the engine.
	 */
	class DrawCallManager {
	public:
		DrawCallManager() {}

		/**
		 * Begin drawing.
		 * This must be called before updating any objects which are submitted to the draw queue.
		 */
		void BeginDraw();

		/**
		 * Submit draw commands to the GPU.
		 * This function call actually draws objects to the scene and should be done at the end of the iteration.
		 */
		void SubmitCommands();
	};
}