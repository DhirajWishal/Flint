// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Flint.hpp"

#ifdef FLINT_PLATFORM_WINDOWS
#include "VulkanBackend/VulkanDevice.hpp"

#endif

namespace Flint
{
	std::shared_ptr<Flint::Backend::Instance> CreateInstance(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation)
	{
#ifdef FLINT_PLATFORM_WINDOWS
		return std::make_shared<Backend::VulkanInstance>(std::move(applicationName), applicationVersion, enableBackendValidation);

#endif
	}
}