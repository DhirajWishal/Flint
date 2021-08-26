// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.hpp"

#include "Flint/Instance.hpp"

Application::Application()
{
	pInstance = Flint::CreateInstance(true);
	pDevice = pInstance->CreateDevice(Flint::DeviceFlags::EXTERNAL | Flint::DeviceFlags::COMPUTE_COMPATIBLE | Flint::DeviceFlags::GRAPHICS_COMPATIBLE);
	pDisplay = pInstance->CreateDisplay(Flint::FBox2D(1280, 720), "Flint Editor");

	// Initialize the main render target.
	mRenderTarget.Initialize(pDevice, pDisplay);

	// Main execution loop.
	while (pDisplay->IsOpen() && bShouldRun)
		Execute();
}

Application::~Application()
{
	pInstance->DestroyDisplay(pDisplay);
	pInstance->DestroyDevice(pDevice);
	pInstance->Terminate();
}

void Application::Execute()
{
	pDisplay->Update();
}
