// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Reactors/VulkanReactor.hpp"
#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	class VulkanReactor final
	{
	public:
		VulkanReactor(std::list<std::unique_ptr<CommandBase>>& commands, std::mutex& mutex) : mCommands(commands), mResourceLock(mutex), mInstance(true)
		{
		}

	private:
		std::list<std::unique_ptr<CommandBase>>& mCommands;
		std::mutex& mResourceLock;

		VulkanBackend::VulkanInstance mInstance;
	};

	void VulkanReactorWorker(std::list<std::unique_ptr<CommandBase>>& commands, std::mutex& mutex)
	{
		VulkanReactor reactor = { commands, mutex };
	}
}