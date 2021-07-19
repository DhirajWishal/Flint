// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Instance.hpp"

#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	std::shared_ptr<Instance> CreateInstance(bool enableValidation)
	{
		return std::make_shared<VulkanBackend::VulkanInstance>(enableValidation);
	}

	void DestroyInstance(const std::shared_ptr<Instance>& pInstance)
	{
		pInstance->Terminate();
	}
}