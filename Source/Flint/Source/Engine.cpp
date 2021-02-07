// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Engine.h"
#include "Core/ErrorHandler/Logger.h"

#include "VulkanBackend/VulkanInstance.h"

namespace Flint
{
	void Engine::Initialize(BackenAPI api, bool enableValidation, UI32 width, UI32 height, const char* pTitle)
	{
		mAPI = api;

		switch (api)
		{
		case Flint::BackenAPI::VULKAN:
			pInstance = new VulkanBackend::VulkanInstance();
			break;
		case Flint::BackenAPI::DIRECT_X_12:
			break;
		case Flint::BackenAPI::WEB_GPU:
			break;
		default:
			FLINT_LOG_ERROR(TEXT("Invalid or Undefined backend API type!"))
				break;
		}

		pInstance->Initialize(enableValidation);
		pDisplay = pInstance->CreateDisplay(width, height, pTitle);
		pDevice = pDisplay->CreatDevice();
	}

	void Engine::Terminate()
	{
		pRenderTarget->Terminate();
		delete pRenderTarget;

		pDevice->Terminate();
		delete pDevice;

		pDisplay->Terminate();
		delete pDisplay;

		pInstance->Terminate();
		delete pInstance;
	}

	void Engine::SwitchAPI(BackenAPI newAPI)
	{
	}

	void Engine::CreateRenderTarget(const Vector2& extent, UI32 bufferCount)
	{
		pRenderTarget = pDevice->CreateRenderTarget(Backend::RenderTargetType::SCREEN_BOUND, extent, bufferCount);
	}
}