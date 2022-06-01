// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan worker function.
		 * This function will execute the Vulkan backend and will react to the submitted commands.
		 *
		 * @param conditionalVariable The conditional variable stating if it should be blocked or not.
		 * @param resourceLock The resource mutex.
		 * @param commands The command storage containing all the commands.
		 * @param createInstance The create instance structure containing information to create the instance.
		 */
		void Worker(std::condition_variable& conditionalVariable, std::mutex& resourceLock, CommandStorage& commands, Commands::CreateInstance&& createInstance);
	}
}