// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FInstance.h"

#include "VulkanBackend\VulkanInstance.h"

#include "Core\ErrorHandler\Logger.h"

namespace Flint
{
	FInstance::FInstance(InstanceAPI api, bool enableValidation)
		: mAPI(api)
	{
		switch (api)
		{
		case Flint::InstanceAPI::VULKAN:
			pInstance = std::make_unique<VulkanBackend::VulkanInstance>(enableValidation);
			break;
		case Flint::InstanceAPI::WEB_GPU:
			break;
		case Flint::InstanceAPI::DIRECT_X_12:
			break;
		default:
			Logger::LogError(TEXT("Invalid or undefined Instance API type!"));
			break;
		}
	}
	
	void FInstance::Terminate()
	{
		pInstance->Terminate();
	}
}