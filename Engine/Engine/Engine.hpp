// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Scene.hpp"

namespace Flint
{
	/**
	 * Engine class.
	 * This class is the core engine object and is used to create other objects.
	 */
	class Engine
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param applicationName The name of the application.
		 * @param applicationVersion The version of the application.
		 * @param enableBackendValidation Whether or not to enable backend validation. Turn this to true ONLY in the debug build.
		 */
		explicit Engine(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation);

		/**
		 * Virtual destructor.
		 */
		virtual ~Engine();

		/**
		 * Destroy the engine instance.
		 */
		virtual void destroy();

	protected:
		Backend::Instance m_Instance;
		Backend::Device m_Device;
	};
}