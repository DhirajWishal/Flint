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
		pInstance = std::make_shared<VulkanBackend::VulkanInstance>(enableValidation);
	}

	void GraphicsBackendObject::TerminateInstance()
	{
		pInstance->Terminate();
	}

	void GraphicsBackendObject::InitializeDisplay(UI32 width, UI32 height, const char* pTitle)
	{
		pDisplay = pInstance->CreateDisplay(Vector2(static_cast<float>(width), static_cast<float>(height)), pTitle);
	}

	void GraphicsBackendObject::TerminateDisplay()
	{
		pDisplay->Terminate();
	}

	void GraphicsBackendObject::InitializeDevice()
	{
		pDevice = pInstance->CreateDevice();
	}

	void GraphicsBackendObject::TerminateDevice()
	{
		pDevice->Terminate();
	}
}