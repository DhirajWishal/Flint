// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBackendHandler.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"

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

			m_pEngines.clear();
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
				case CommandStorage::IndexOf<Commands::Terminate>():
					return false;

				case CommandStorage::IndexOf<Commands::CreateDevice>():
					std::get<Commands::CreateDevice>(variant).m_Promise.set_value(IntToEnum<DeviceHandle>(m_pEngines.emplace(std::make_unique<VulkanEngine>(m_Instance)).first));
					break;

				case CommandStorage::IndexOf<Commands::DestroyDevice>():
				{
					auto& command = std::get<Commands::DestroyDevice>(variant);
					m_pEngines.remove(EnumToInt(command.m_DeviceHandle));
					command.m_Promise.set_value();
					break;
				}

				default:
					break;
				}
			}
			catch (...) {}

			return true;
		}
	}
}