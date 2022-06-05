// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBackendHandler.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanWindow.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBackendHandler::VulkanBackendHandler(std::condition_variable& conditionalVariable, std::mutex& resourceLock, CommandStorage& commands, Commands::CreateInstance&& createInstance) try
			: m_Instance(std::move(createInstance.m_ApplicationName), createInstance.m_ApplicationVersion, createInstance.m_EnableValidation)
			, m_ConditionalVariable(conditionalVariable)
			, m_ResouceMutex(resourceLock)
			, m_CommandStorage(commands)
			, m_Lock(m_ResouceMutex)
		{
			createInstance.m_Promise.set_value(IntToEnum<InstanceHandle>(reinterpret_cast<std::underlying_type_t<InstanceHandle>>(&m_Instance)));
		}
		catch (...)
		{
			try
			{
				createInstance.m_Promise.set_exception(std::current_exception());
			}
			catch (...) {}
		}

		VulkanBackendHandler::~VulkanBackendHandler()
		{
			// Unlock resources now, we can finish everything.
			m_Lock.unlock();

			// Lets finish all the commands if we have any so we don't miss something important.
			while (!m_CommandStorage.empty())
			{
				// Get and execute the command.
				executeCommand(std::move(m_CommandStorage.pop()));
			}

			m_pRasterizers.clear();
			m_pWindows.clear();
			m_pDevices.clear();
		}

		void VulkanBackendHandler::run()
		{
			bool shouldRun = true;
			do
			{
				// Wait until we have something to execute.
				m_ConditionalVariable.wait(m_Lock, [this] { return !m_CommandStorage.empty(); });

				// Execute the commands if we have any.
				if (!m_CommandStorage.empty())
				{
					// Get the front commands and pop it from the storage.
					auto command = std::move(m_CommandStorage.pop());

					// Unlock the resources till we complete the task.
					m_Lock.unlock();

					// Execute it depending on the command submitted.
					shouldRun = executeCommand(std::move(command));

					// Lock em back up.
					m_Lock.lock();
				}

			} while (shouldRun);
		}

		bool VulkanBackendHandler::executeCommand(CommandVariant&& variant)
		{
			// Global try catch because we can't resort to stopping the whole system if an exception was thrown.
			try
			{
				switch (variant.index())
				{
				case CommandStorage::IndexOf<Commands::Synchronize>():
					std::get<Commands::Synchronize>(variant).m_Promise.set_value();
					break;

				case CommandStorage::IndexOf<Commands::Terminate>():
					std::get<Commands::Terminate>(variant).m_Promise.set_value();
					return false;

				case CommandStorage::IndexOf<Commands::CreateDevice>():
					createDevice(std::move(std::get<Commands::CreateDevice>(variant)));
					break;

				case CommandStorage::IndexOf<Commands::DestroyDevice>():
					destroyDevice(std::move(std::get<Commands::DestroyDevice>(variant)));
					break;

				case CommandStorage::IndexOf<Commands::CreateWindow>():
					createWindow(std::move(std::get<Commands::CreateWindow>(variant)));
					break;

				case CommandStorage::IndexOf<Commands::UpdateWindow>():
					updateWindow(std::move(std::get<Commands::UpdateWindow>(variant)));
					break;

				case CommandStorage::IndexOf<Commands::DestroyWindow>():
					destroyWindow(std::move(std::get<Commands::DestroyWindow>(variant)));
					break;

				case CommandStorage::IndexOf<Commands::CreateRasterizer>():
					createRasterizer(std::move(std::get<Commands::CreateRasterizer>(variant)));
					break;

				case CommandStorage::IndexOf<Commands::DestroyRasterizer>():
					destroyRasterizer(std::move(std::get<Commands::DestroyRasterizer>(variant)));
					break;

				default:
					break;
				}
			}
			catch (...) {}

			return true;
		}

		void VulkanBackendHandler::createDevice(Commands::CreateDevice&& command)
		{
			command.m_Promise.set_value(IntToEnum<DeviceHandle>(m_pDevices.emplace(std::make_unique<VulkanDevice>(m_Instance)).first));
		}

		void VulkanBackendHandler::destroyDevice(Commands::DestroyDevice&& command)
		{
			m_pDevices.remove(EnumToInt(command.m_DeviceHandle));
			command.m_Promise.set_value();
		}

		void VulkanBackendHandler::createWindow(Commands::CreateWindow&& command)
		{
			auto& device = *m_pDevices[EnumToInt(command.m_DeviceHandle)];
			command.m_Promise.set_value(IntToEnum<WindowHandle>(
				m_pWindows.emplace(
					std::make_unique<VulkanWindow>(
						device,
						std::move(command.m_Title),
						command.m_Width,
						command.m_Height)
				).first)
			);
		}

		void VulkanBackendHandler::updateWindow(Commands::UpdateWindow&& command)
		{
			auto& window = *m_pWindows[EnumToInt(command.m_WindowHandle)];
			window.update();

			command.m_Promise.set_value();
		}

		void VulkanBackendHandler::destroyWindow(Commands::DestroyWindow&& command)
		{
			m_pWindows.remove(EnumToInt(command.m_WindowHandle));
			command.m_Promise.set_value();
		}

		void VulkanBackendHandler::createRasterizer(Commands::CreateRasterizer&& command)
		{
			auto& device = *m_pDevices[EnumToInt(command.m_DeviceHandle)];
			command.m_Promise.set_value(
				IntToEnum<RasterizerHandle>(
					m_pRasterizers.emplace(
						std::make_unique<VulkanRasterizer>(
							device,
							command.m_Width,
							command.m_Height,
							command.m_FrameCount,
							std::move(command.m_AttachmentDescriptions),
							command.m_MultisampleCount,
							command.m_ExclusiveBuffering)
					).first)
			);
		}

		void VulkanBackendHandler::destroyRasterizer(Commands::DestroyRasterizer&& command)
		{
			m_pRasterizers.remove(EnumToInt(command.m_RasterizerHandle));
			command.m_Promise.set_value();
		}
	}
}