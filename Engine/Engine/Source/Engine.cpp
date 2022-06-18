// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Engine.hpp"

namespace Flint
{
	Engine::Engine(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation)
		: m_Instance(std::move(applicationName), applicationVersion, enableBackendValidation)
		, m_Device(m_Instance)
	{

	}

	Engine::~Engine()
	{
		if (m_Device.isValid())
			m_Device.terminate();

		if (m_Instance.isValid())
			m_Instance.terminate();
	}

	void Engine::destroy()
	{
		m_Device.terminate();
		m_Instance.terminate();
	}
}