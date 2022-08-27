// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Backend/StaticInitializer.hpp"

#include <SDL.h>

namespace Flint
{
	namespace Backend
	{
		StaticInitializer::StaticInitializer()
		{
			// Initialize SDL.
			SDL_Init(SDL_INIT_VIDEO);
		}

		StaticInitializer::~StaticInitializer()
		{
			// Quit SDL.
			SDL_Quit();
		}
	}
}