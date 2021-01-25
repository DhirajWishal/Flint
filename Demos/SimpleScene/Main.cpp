// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/Backend.h"
#include "LowLevel/Device.h"
#include "LowLevel/RenderTargets/RenderTargetSB3D.h"
#include "Core/Benchmark/Timer.h"

#include <thread>
#include <Windows.h>

int main()
{
	Flint::Benchmark::Timer t;
	Flint::LowLevel::SetupBackend(Flint::LowLevel::BackendAPI::VULKAN);

	Flint::LowLevel::Instance instance = {};

#ifdef FLINT_DEBUG
	instance.Initialize(true);

#else
	instance.Initialize(false);

#endif // FLINT_DEBUG

	Flint::LowLevel::Display display = {};
	display.Initialize(instance, 1280, 720, "Flint Demo: Simple Scene");

	Flint::LowLevel::Device device = {};
	device.Initialize(display);

	Flint::LowLevel::RenderTargetSB3D renderTarget = {};
	renderTarget.Initialize(device, Flint::Vector2(1280, 720));

	//std::this_thread::sleep_for(std::chrono::microseconds(5000000));

	renderTarget.Terminate();
	device.Terminate();

	display.Terminate();
	instance.Terminate();

	return 0;
}