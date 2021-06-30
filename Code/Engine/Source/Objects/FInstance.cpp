// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Objects/FInstance.hpp"

#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	namespace _Helpers
	{
		inline Backend::Instance& CreateInstance(BackendAPI api, bool enableValidation)
		{
			return *new VulkanBackend::VulkanInstance(enableValidation);
		}
	}

	FInstance::FInstance(BackendAPI api, bool enableValidation)
		: mInstance(_Helpers::CreateInstance(api, enableValidation)), mAPI(api)
	{
	}

	FInstance::~FInstance()
	{
		pInstance->Terminate();
		delete& mInstance;
	}
}