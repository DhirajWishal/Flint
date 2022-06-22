// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Scene.hpp"

#include <string>
#include <memory>

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
		explicit Engine(const std::string& applicationName, uint32_t applicationVersion, bool enableBackendValidation) {}

		/**
		 * Virtual destructor.
		 */
		virtual ~Engine() {}

		/**
		 * Destroy the engine instance.
		 */
		virtual void destroy() = 0;
	};

	/**
	 * Create a new engine.
	 * An application requires just one instance of this.
	 *
	 * @param applicationName The name of the application.
	 * @param applicationVersion The version of the application.
	 * @param enableBackendValidation Whether or not to enable backend validation. Turn this to true ONLY in the debug build.
	 * @return The created engine pointer.
	 */
	std::shared_ptr<Engine> CreateEngine(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation);
}