// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace Flint
{
	namespace Backend
	{
		/**
		 * Static initializer struct.
		 * This structure is used to initialize different things in the engine.
		 */
		struct StaticInitializer
		{
			/**
			 * Default constructor.
			 */
			StaticInitializer();

			/**
			 * Destructor.
			 */
			~StaticInitializer();
		};
	}
}