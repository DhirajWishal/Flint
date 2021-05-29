// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/FBackend.h"
#include <memory>

int main()
{
	std::unique_ptr<Flint::FBackend> pBackend = std::unique_ptr<Flint::FBackend>(Flint::CreateBackend(Flint::BackendAPI::VULKAN, true));
	Flint::FDeviceHandle device = pBackend->CreateDevice();
	Flint::FDisplayHandle display = pBackend->CreateDisplay(1280, 720, "Flint-Demo: Simple Scene");

	pBackend->DestroyDisplay(display);
	pBackend->DestroyDevice(device);
	pBackend->Terminate();
	return 0;
}