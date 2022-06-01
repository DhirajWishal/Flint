// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBackend.hpp"
#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Execute command function.
		 * This function executes a command sent by the client.
		 *
		 * @param variant The variant containing the command.
		 * @return Whether we could continue execution.
		 */
		bool ExecuteCommand(CommandVariant&& variant)
		{
			// Global try catch because we can't resort to stopping the whole system if an exception was thrown.
			try
			{
				switch (variant.index())
				{
				case 0:
					return false;

				case 1:
					break;

				default:
					break;
				}
			}
			catch (...) {}

			return true;
		}

		void Worker(std::condition_variable& conditionalVariable, std::mutex& resourceLock, CommandStorage& commands, Commands::CreateInstance&& createInstance)
		{
			bool shouldRun = true;
			auto lock = std::unique_lock(resourceLock);
			std::unique_ptr<VulkanInstance> pVulkanInstance = nullptr;

			// Try and create the Vulkan instance.
			try
			{
				pVulkanInstance = std::make_unique<VulkanInstance>(std::move(createInstance.m_ApplicationName), createInstance.m_ApplicationVersion, createInstance.m_EnableValidation);
				createInstance.m_Promise.set_value(IntToEnum<InstanceHandle>(reinterpret_cast<std::underlying_type_t<InstanceHandle>>(pVulkanInstance.get())));
			}
			catch (...)
			{
				// Setting an exception could also throw.
				try
				{
					// Notify the user that we couldn't create the instance, and stop execution because we can't continue without it.
					createInstance.m_Promise.set_exception(std::current_exception());
				}
				catch (...)
				{
				}

				return;
			}

			do
			{
				// Wait until we have something to execute.
				conditionalVariable.wait(lock, [&commands] { return !commands.empty(); });

				// Execute the commands if we have any.
				if (!commands.empty())
				{
					// Get the front commands and pop it from the storage.
					auto command = std::move(commands.front());
					commands.pop_front();

					// Unlock the resources till we complete the task.
					lock.unlock();

					// Execute it depending on the command submitted.
					shouldRun = ExecuteCommand(std::move(command));

					// Lock em back up.
					lock.lock();
				}

			} while (shouldRun);

			// Unlock resources now, we can finish everything.
			lock.unlock();

			// Lets finish all the commands if we have any so we don't miss something important.
			while (!commands.empty())
			{
				// Get the command and remove it from the list.
				auto command = std::move(commands.front());
				commands.pop_front();

				// Execute the command.
				ExecuteCommand(std::move(command));
			}
		}
	}
}