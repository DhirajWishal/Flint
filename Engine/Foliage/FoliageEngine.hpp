// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Engine.hpp"

namespace Flint
{
	/**
	 * Foliage engine class.
	 * The foliage engine is the PBR based, ray tracing engine which is designed to reproduce realistic looking graphics.
	 */
	class FoliageEngine final : public Engine
	{
	public:
		// Using the same constructor as the super class.
		using Engine::Engine;

		/**
		 * Destructor.
		 */
		~FoliageEngine() override;

		/**
		 * Destroy the engine.
		 */
		void destroy() override;
	};
}