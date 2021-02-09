// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Benchmark/Timer.h"
#include "Flint/Engine.h"
#include "Flint/Components/WireFrame.h"

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

	engine.CreateRenderTarget(Flint::Vector2(1280, 720));

	Flint::WireFrame wireframe = {};
	wireframe.LoadFromFile("E:\\Flint\\Assets\\Models\\Cube\\Cube.obj", Flint::CreateDefaultAttributes());

	engine.Terminate();
	return 0;
}
