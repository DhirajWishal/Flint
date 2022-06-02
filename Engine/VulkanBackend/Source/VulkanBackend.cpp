// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBackend.hpp"
#include "VulkanBackend/VulkanBackendHandler.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		void Worker(std::condition_variable& conditionalVariable, std::mutex& resourceLock, CommandStorage& commands, Commands::CreateInstance&& createInstance)
		{
			VulkanBackendHandler(conditionalVariable, resourceLock, commands, std::move(createInstance)).run();
		}
	}
}