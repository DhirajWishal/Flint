// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Vulkan/VulkanEngine.hpp"

namespace Flint
{
	VulkanEngine::VulkanEngine(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation)
		: Engine(applicationName, applicationVersion, enableBackendValidation)
		, m_Instance(std::move(applicationName), applicationVersion, enableBackendValidation)
		, m_Device(m_Instance)
	{

	}

	VulkanEngine::~VulkanEngine()
	{
	}

	void VulkanEngine::destroy()
	{
		m_Device.terminate();
		m_Instance.terminate();
	}
}