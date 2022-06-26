// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Engine.hpp"

#ifdef FLINT_PLATFORM_WINDOWS
#include "VulkanBackend/VulkanDevice.hpp"

#endif

namespace Flint
{
	static std::shared_ptr<Engine> g_Engine = nullptr;

	Engine::Engine(const std::string& applicationName, uint32_t applicationVersion, bool enableBackendValidation)
	{
#ifdef FLINT_PLATFORM_WINDOWS
		auto pInstance = std::make_shared<VulkanBackend::VulkanInstance>(std::move(applicationName), applicationVersion, enableBackendValidation);
		m_pDevice = pInstance->createDevice();

#endif
	}

	void Engine::Create(const std::string& applicationName, uint32_t applicationVersion, bool enableBackendValidation)
	{
		g_Engine = std::make_shared<Engine>(std::move(applicationName), applicationVersion, enableBackendValidation);
	}

	std::shared_ptr<Flint::Engine> CreateEngine(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation)
	{
#ifdef FLINT_PLATFORM_WINDOWS
		return nullptr;
		//return std::make_shared<VulkanEngine>(std::move(applicationName), applicationVersion, enableBackendValidation);

#endif
	}

	std::shared_ptr<Flint::Instance> CreateInstance(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation)
	{
#ifdef FLINT_PLATFORM_WINDOWS
		return std::make_shared<VulkanBackend::VulkanInstance>(std::move(applicationName), applicationVersion, enableBackendValidation);

#endif
	}
}