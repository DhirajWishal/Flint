// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

namespace Flint
{
	/**
	 * Flint engine.
	 * This object acts as a command hub as the actual graphics subsystem (FlintSubsystem) is being executed on another thread.
	 */
	class Engine
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param enableValidation: Whether or not to enable backend validation.
		 */
		Engine(bool enableValidation);

		/**
		 * Terminate the engine.
		 */
		void Terminate();

	private:
		/**
		 * Run flint subsystem function.
		 * This function will be executed on another thread and will carry out the graphics operations.
		 */
		static void RunFlintSubsystem(Instance& instance);
	};
}