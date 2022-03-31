// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Vulkan/VulkanReactor.hpp"
#include "Graphics/Vulkan/VulkanEngine.hpp"
#include "VulkanBackend/VulkanDevice.hpp"

namespace Flint
{
	VulkanReactor::VulkanReactor(GraphicsEngine* pEngine, DeviceFlags flags)
		: GraphicsReactor(pEngine, flags)
	{
	}

	void VulkanReactor::Worker(GraphicsEngine* pEngine, DeviceFlags flags)
	{
		// Create the device, we need it.
		const auto m_pDevice = static_cast<VulkanEngine*>(pEngine)->GetInstance()->CreateDevice(flags);
		auto lock = std::unique_lock(m_Mutex);

		do
		{
			// Wait till we have a command or should stop execution.
			m_ConditionalVariable.wait(lock, [this] { return !m_Commands.empty() || m_bShouldStop; });

			// If we have something to execute, lets execute one command.
			if (!m_Commands.empty())
			{
				auto command = std::move(m_Commands.front());
				m_Commands.pop_back();

				// Unlock resources now that we can execute the command.
				lock.unlock();

				// Execute the command.
				command();

				// Lock everything back in place.
				lock.lock();
			}
		} while (!m_bShouldStop);

		// Unlock resources now, we can finish everything.
		lock.unlock();

		// Lets execute the remaining commands.
		for (const auto& command : m_Commands)
			command();
	}
}