// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Benchmark/Timer.h"
#include "Flint/Engine.h"

#include <thread>

int main()
{
	Flint::Benchmark::Timer t;
	Flint::Engine engine = {};
#ifdef FLINT_DEBUG
	engine.Initialize(Flint::BackenAPI::VULKAN, true, 1280, 720, "Flint: Sample Scene");

#else
	engine.Initialize(Flint::BackenAPI::VULKAN, false, 1280, 720, "Flint: Sample Scene");

#endif // FLINT_DEBUG

	engine.CreateRenderTarget({ 1280, 720 });

	engine.Terminate();
	return 0;
}
