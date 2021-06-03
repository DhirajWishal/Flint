// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanInstance.hpp"
#include "Flint/Core/Error.hpp"

#include "VulkanDevice.hpp"

#include <GLFW/glfw3.h>

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Callback
		{
			void GLFWErrorCallback(int code, const char* message)
			{
				if (code != GLFW_NO_ERROR)
					FLINT_THROW_RUNTIME_ERROR(message);
			}
		}

		VulkanInstance::VulkanInstance(bool enableValidation) : Instance(enableValidation)
		{
			InitializeGLFW();

			// Check if Vulkan is supported.
			if (glfwVulkanSupported() != GLFW_TRUE)
				FLINT_THROW_RUNTIME_ERROR("Vulkan is not supported! Make sure that the drivers support Vulkan before using the Vulkan Backend.");

			InitializeInstance();

			if (IsValidationEnabled())
				InitializeDebugger();

			VkResult;
		}

		Device& VulkanInstance::CreateDevice(DeviceFlags flags)
		{
			return *new VulkanDevice(*this);
		}

		void VulkanInstance::Terminate()
		{
			if (IsValidationEnabled())
				TerminateDebugger();

			TerminateInstance();
			TerminateGLFW();
		}

		void VulkanInstance::InitializeGLFW()
		{
			glfwInit();
			glfwSetErrorCallback(_Callback::GLFWErrorCallback);
		}

		void VulkanInstance::TerminateGLFW()
		{
			glfwTerminate();
		}

		void VulkanInstance::InitializeInstance()
		{
		}

		void VulkanInstance::TerminateInstance()
		{
			vkDestroyInstance(vInstance, nullptr);
		}

		void VulkanInstance::InitializeDebugger()
		{
		}

		void VulkanInstance::TerminateDebugger()
		{
		}
	}
}