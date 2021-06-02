// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanInstance.h"
#include "Flint/Core/Error.h"

#include <GLFW/glfw3.h>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanInstance::VulkanInstance(bool enableValidation) : Instance(enableValidation)
		{
			InitializeGLFW();

			// Check if Vulkan is supported.
			if (glfwVulkanSupported() != GLFW_TRUE)
				FLINT_THROW_RUNTIME_ERROR("Vulkan is not supported! Make sure that the drivers support Vulkan before using the Vulkan Backend.");
		}

		void VulkanInstance::Terminate()
		{
			TerminateGLFW();
		}

		void VulkanInstance::InitializeGLFW()
		{
			glfwInit();
		}

		void VulkanInstance::TerminateGLFW()
		{
			glfwTerminate();
		}
	}
}