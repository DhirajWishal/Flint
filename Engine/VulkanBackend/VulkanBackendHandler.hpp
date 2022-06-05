// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"
#include "VulkanRasterizer.hpp"

#include "Core/Commands.hpp"
#include "Core/Containers/SparseArray.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan backend handler class.
		 * This class handles all the commands passed to the worker thread where the Vulkan backend runs in.
		 */
		class VulkanBackendHandler final
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param conditionalVariable The conditional variable stating if it should be blocked or not.
			 * @param resourceLock The resource mutex.
			 * @param commands The command storage containing all the commands.
			 * @param createInstance The create instance structure containing information to create the instance.
			 */
			explicit VulkanBackendHandler(std::condition_variable& conditionalVariable, std::mutex& resourceLock, CommandStorage& commands, Commands::CreateInstance&& createInstance);

			/**
			 * Destructor.
			 */
			~VulkanBackendHandler();

			/**
			 * Run the backend.
			 */
			void run();

		private:
			/**
			 * Execute command function.
			 * This function executes a command sent by the client.
			 *
			 * @param variant The variant containing the command.
			 * @return Whether we could continue execution.
			 */
			bool executeCommand(CommandVariant&& variant);

			/**
			 * Create a new device.
			 *
			 * @param command The command which was submitted by the client.
			 */
			void createDevice(Commands::CreateDevice&& command);

			/**
			 * Destroy a device.
			 *
			 * @parm command The command which was sent by the client.
			 */
			void destroyDevice(Commands::DestroyDevice&& command);

			/**
			 * Create a new window.
			 *
			 * @param command The command which was submitted by the client.
			 */
			void createWindow(Commands::CreateWindow&& command);

			/**
			 * Update a window.
			 * 
			 * @param command The command which was submitted by the client.
			 */
			void updateWindow(Commands::UpdateWindow&& command);

			/**
			 * Destroy a window.
			 *
			 * @parm command The command which was sent by the client.
			 */
			void destroyWindow(Commands::DestroyWindow&& command);

			/**
			 * Create a new rasterizer.
			 *
			 * @param command The command which was submitted by the client.
			 */
			void createRasterizer(Commands::CreateRasterizer&& command);

			/**
			 * Destroy a rasterizer.
			 *
			 * @parm command The command which was sent by the client.
			 */
			void destroyRasterizer(Commands::DestroyRasterizer&& command);

		private:
			VulkanInstance m_Instance;

			std::condition_variable& m_ConditionalVariable;
			std::mutex& m_ResouceMutex;
			CommandStorage& m_CommandStorage;

			std::unique_lock<std::mutex> m_Lock;

		private:
			SparseArray<std::unique_ptr<VulkanDevice>, std::underlying_type_t<DeviceHandle>> m_pDevices;
			SparseArray<std::unique_ptr<VulkanWindow>, std::underlying_type_t<WindowHandle>> m_pWindows;
			SparseArray<std::unique_ptr<VulkanRasterizer>, std::underlying_type_t<RasterizerHandle>> m_pRasterizers;
		};
	}
}