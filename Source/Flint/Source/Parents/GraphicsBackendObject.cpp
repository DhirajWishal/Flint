// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Parents/GraphicsBackendObject.h"
#include "Core/ErrorHandler/Logger.h"

#include "VulkanBackend/VulkanInstance.h"

namespace Flint
{
	void GraphicsBackendObject::InitializeInstance(BackendAPI api, bool enableValidation)
	{
		mAPI = api;
		mInstance.Initialize(enableValidation);
	}

	void GraphicsBackendObject::TerminateInstance()
	{
		mInstance.Terminate();
	}

	void GraphicsBackendObject::InitializeDisplay(UI32 width, UI32 height, const char* pTitle)
	{
		mDisplay.Initialize(&mInstance, Vector2(static_cast<float>(width), static_cast<float>(height)), pTitle);
	}

	void GraphicsBackendObject::TerminateDisplay()
	{
		mDisplay.Terminate();
	}

	void GraphicsBackendObject::InitializeDevice()
	{
		mDevice.Initialize(&mInstance);
	}

	void GraphicsBackendObject::TerminateDevice()
	{
		mDevice.Terminate();
	}
}
