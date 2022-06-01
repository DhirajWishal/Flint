// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend.hpp"
#include "Core/Instance.hpp"

#include <thread>

namespace Flint
{
	/**
	 * Graphcis engine class.
	 * This contains all the commands and other utilities to communicate with the backend.
	 */
	class GraphicsEngine final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param applicationName The name of the application.
		 * @param applicationVersion The application's version.
		 * @param enableValidation Whether or not to enable backend validation.
		 * @param backendAPI The backend API to use. Note that this is highly platform specific! If multiple is provided, the best is selected (which will be Vulkan ofc :))
		 */
		explicit GraphicsEngine(std::string&& applicationName, uint32_t applicationVersion, bool enableValidation, BackendAPI backendAPI);

		/**
		 * Destructor.
		 */
		~GraphicsEngine();

		/**
		 * Get all the supported backend(s).
		 *
		 * @return The Backend API.
		 */
		[[nodiscard]] static BackendAPI GetSupportedBackends();

	private:
		/**
		 * Issue a command to the system.
		 *
		 * @param variant The variant to send.
		 */
		void issueCommand(CommandVariant&& variant);

	private:
		std::jthread m_WorkerThread;
		std::condition_variable m_ConditionalVariable;
		std::mutex m_ResouceMutex;
		CommandStorage m_Commands;

		InstanceHandle m_Instance;
	};
}