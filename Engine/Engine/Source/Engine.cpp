// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Engine.hpp"

#ifdef FLINT_PLATFORM_WINDOWS
#include "Engine/Vulkan/VulkanEngine.hpp"

#endif

namespace Flint
{
	std::shared_ptr<Flint::Engine> CreateEngine(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation)
	{
#ifdef FLINT_PLATFORM_WINDOWS
		return std::make_shared<VulkanEngine>(std::move(applicationName), applicationVersion, enableBackendValidation);

#endif
	}
}