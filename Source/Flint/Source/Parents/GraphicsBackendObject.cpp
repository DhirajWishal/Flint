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

		switch (api)
		{
		case Flint::BackendAPI::VULKAN:
			pInstance = new VulkanBackend::VulkanInstance();
			break;
		case Flint::BackendAPI::DIRECT_X_12:
			break;
		case Flint::BackendAPI::WEB_GPU:
			break;
		default:
			FLINT_LOG_ERROR(TEXT("Invalid or Undefined backend API type!"))
				break;
		}

		pInstance->Initialize(enableValidation);
	}

	void GraphicsBackendObject::TerminateInstance()
	{
		pInstance->Terminate();
		delete pInstance;
	}

	void GraphicsBackendObject::InitializeDisplay(UI32 width, UI32 height, const char* pTitle)
	{
		pDisplay = pInstance->CreateDisplay(width, height, pTitle);
	}

	void GraphicsBackendObject::TerminateDisplay()
	{
		pDisplay->Terminate();
		delete pDisplay;
	}

	void GraphicsBackendObject::InitializeDevice()
	{
		pDevice = pDisplay->CreateDevice();
	}

	void GraphicsBackendObject::TerminateDevice()
	{
		pDevice->Terminate();
		delete pDevice;
	}
}
