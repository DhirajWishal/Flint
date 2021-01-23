// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "FlintLowLevel/Backend.h"
#include "FlintLowLevel/Display.h"

#include <thread>

int main()
{
	Flint::LowLevel::SetupBackend(Flint::LowLevel::BackendAPI::VULKAN);

	Flint::LowLevel::Instance instance = {};

#ifdef FLINT_DEBUG
	instance.Initialize(true);

#else
	instance.Initialize(false);

#endif // FLINT_DEBUG

	Flint::LowLevel::Display display = {};
	display.Initialize(instance, 1280, 720, "Flint Demo: Simple Scene");

	std::this_thread::sleep_for(std::chrono::microseconds(5000000));

	display.Terminate();
	instance.Terminate();

	return 0;
}