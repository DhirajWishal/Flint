// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Foliage/FoliageEngine.hpp"

namespace Flint
{
	FoliageEngine::~FoliageEngine()
	{
		destroy();
	}

	void FoliageEngine::destroy()
	{
		// Destroy the parent.
		Engine::destroy();
	}
}