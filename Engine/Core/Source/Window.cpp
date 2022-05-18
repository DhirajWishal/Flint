// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Window.hpp"

#include <SDL.h>

namespace /* anonymous */
{
	/**
	 * Static initializer struct.
	 * This struct is used to initialize and destroy the SDL library.
	 */
	struct StaticInitializer
	{
		/**
		 * Default constructor.
		 */
		StaticInitializer()
		{
			// Initialize SDL.
			SDL_Init(SDL_INIT_VIDEO);
		}

		/**
		 * Destructor.
		 */
		~StaticInitializer()
		{
			// Quit SDL.
			SDL_Quit();
		}
	};
}

namespace Flint
{
	Window::Window(Engine& engine, std::string&& title, uint32_t width /*= -1*/, uint32_t height /*= -1*/)
		: EngineBoundObject(engine), m_Title(std::move(title)), m_Width(width), m_Height(height)
	{
		// Create the static initializer.
		static StaticInitializer initializer;
	}
}