// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.hpp"

#include "GraphicsCore/Instance.hpp"

Application::Application()
{
	pInstance = Flint::CreateInstance(true);
	pDevice = pInstance->CreateDevice(Flint::DeviceFlags::EXTERNAL | Flint::DeviceFlags::COMPUTE_COMPATIBLE | Flint::DeviceFlags::GRAPHICS_COMPATIBLE);

	// Initialize the main render target.
	mRenderTarget.Initialize(pDevice, pInstance);

	// Main execution loop.
	while (mRenderTarget.IsDisplayOpen() && bShouldRun)
		Execute();
}

Application::~Application()
{
	mRenderTarget.Terminate();

	pDevice->Terminate();
	pInstance->Terminate();
}

void Application::Execute()
{
	mRenderTarget.PollEvents();
}
