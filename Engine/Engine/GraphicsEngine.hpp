// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend.hpp"
#include "Core/Instance.hpp"

#include <thread>

namespace Flint
{
	/**
	 * Graphics engine class.
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
		explicit GraphicsEngine(std::string&& applicationName, uint32_t applicationVersion, bool enableValidation, Core::BackendAPI backendAPI);

		/**
		 * Destructor.
		 */
		~GraphicsEngine();

		/**
		 * Get all the supported backend(s).
		 *
		 * @return The Backend API.
		 */
		[[nodiscard]] static Core::BackendAPI GetSupportedBackends();

	public:
		/**
		 * Create a new graphics device.
		 *
		 * @return The device future.
		 */
		std::future<DeviceHandle> createDevice();

		/**
		 * Destroy a device.
		 *
		 * @param handle The device handle to destroy.
		 * @return The future containing the result.
		 */
		std::future<void> destroyDevice(DeviceHandle handle);

		/**
		 * Create a new window.
		 *
		 * @param handle The device handle.
		 * @param title The title of the window.
		 * @param width The width of the window. Default is -1 (which is for full screen).
		 * @param height The height of the window. Default is -1 (which is for full screen).
		 */
		std::future<WindowHandle> createWindow(DeviceHandle handle, std::string&& title, uint32_t width = -1, uint32_t height = -1);

		/**
		 * Update a window.
		 *
		 * @param handle The window handle to update.
		 */
		std::future<void> updateWindow(WindowHandle handle);

		/**
		 * Destroy a created window.
		 *
		 * @param handle The window handle.
		 * @return The future containing the result.
		 */
		std::future<void> destroyWindow(WindowHandle handle);

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