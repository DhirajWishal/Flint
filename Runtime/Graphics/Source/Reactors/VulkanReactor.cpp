// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Reactors/VulkanReactor.hpp"
#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	class VulkanReactor final
	{
	public:
		VulkanReactor(std::list<std::unique_ptr<CommandBase>>& commands, std::mutex& mutex) : m_Commands(commands), m_ResourceLock(mutex), m_Instance(true)
		{
		}

	private:
		std::list<std::unique_ptr<CommandBase>>& m_Commands;
		std::mutex& m_ResourceLock;

		VulkanBackend::VulkanInstance m_Instance;
	};

	void VulkanReactorWorker(std::list<std::unique_ptr<CommandBase>>& commands, std::mutex& mutex)
	{
		VulkanReactor reactor = { commands, mutex };
	}
}