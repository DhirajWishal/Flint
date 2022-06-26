// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

//#include "Scene.hpp"
#include "Core/Device.hpp"

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
		explicit Engine(const std::string& applicationName, uint32_t applicationVersion, bool enableBackendValidation);

		/**
		 * Create the engine.
		 *
		 * @param applicationName The name of the application.
		 * @param applicationVersion The version of the application.
		 * @param enableBackendValidation Whether or not to enable backend validation. Turn this to true ONLY in the debug build.
		 */
		static void Create(const std::string& applicationName, uint32_t applicationVersion, bool enableBackendValidation);

	private:
		std::shared_ptr<Device> m_pDevice = nullptr;
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
	[[nodiscard]] std::shared_ptr<Engine> CreateEngine(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation);

	/**
	 * Create a new instance.
	 *
	 * @param applicationName The name of the application.
	 * @param applicationVersion The version of the application.
	 * @param enableBackendValidation Whether or not to enable backend validation. Turn this to true ONLY in the debug build.
	 * @return The created instance pointer.
	 */
	[[nodiscard]] std::shared_ptr<Instance> CreateInstance(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation);
}