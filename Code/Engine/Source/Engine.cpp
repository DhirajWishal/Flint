// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine.hpp"
#include "FlintSubsystem.h"

namespace Flint
{
	Engine::Engine(bool enableValidation)
	{
	}

	void Engine::Terminate()
	{
	}

	void Engine::RunFlintSubsystem(Instance& instance)
	{
		FlintSubsystem subsystem = FlintSubsystem(instance);

		bool bShouldRun = true;
		do
		{

		} while (bShouldRun);
	}
}