// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanInstance.hpp"
#include "VulkanMacros.hpp"

#include "VulkanDevice.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			void GLFWErrorCallback(int code, const char* message)
			{
				if (code != GLFW_NO_ERROR)
					FLINT_THROW_RUNTIME_ERROR(message);
			}

			bool CheckValidationLayerSupport(std::vector<const char*> layers)
			{
				UI32 layerCount = 0;
				vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

				std::vector<VkLayerProperties> availableLayers(layerCount);
				vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

				for (const char* layerName : layers)
				{
					bool layerFound = false;
					for (const VkLayerProperties& layerProperties : availableLayers)
					{
						if (strcmp(layerName, layerProperties.layerName) == 0)
						{
							layerFound = true;
							break;
						}
					}

					if (!layerFound)
						return false;
				}

				return true;
			}

			std::vector<const char*> GetRequiredInstanceExtensions(bool enableValidation)
			{
				UI32 glfwExtentionCount = 0;
				const char** glfwExtensions = nullptr;
				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);
				std::vector<const char*> extentions(glfwExtensions, glfwExtensions + glfwExtentionCount);

				if (enableValidation)
					INSERT_INTO_VECTOR(extentions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

				//if (pushDescriptorsSupported)
				//	extentions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
				//
				//if (checkpointsSupported)
				//	extentions.push_back(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
				//
				//if (meshShadingSupported)
				//	extentions.push_back(VK_NV_MESH_SHADER_EXTENSION_NAME);
				//
				//extentions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

				return extentions;
			}

			static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData)
			{
				std::string myMessagePreStatement = ": ";

				if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
					myMessagePreStatement = "(General): ";
				else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
					myMessagePreStatement = "(Validation): ";
				else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
					myMessagePreStatement = "(Performance): ";

				std::cout << "Vulkan Validation Layer " << myMessagePreStatement << pCallbackData->pMessage << std::endl;
				return VK_FALSE;
			}

			VkDebugUtilsMessengerCreateInfoEXT CreateDebugMessengerCreateInfo()
			{
				VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				createInfo.pfnUserCallback = DebugCallback;

				createInfo.pNext = VK_NULL_HANDLE;
				createInfo.pUserData = VK_NULL_HANDLE;
				createInfo.flags = VK_NULL_HANDLE;

				return createInfo;
			}
		}

		VulkanInstance::VulkanInstance(bool enableValidation) : Instance(enableValidation)
		{
			InitializeGLFW();

			// Check if Vulkan is supported.
			if (glfwVulkanSupported() != GLFW_TRUE)
				FLINT_THROW_RUNTIME_ERROR("Vulkan is not supported! Make sure that the drivers support Vulkan before using the Vulkan Backend.");

			if (enableValidation)
				INSERT_INTO_VECTOR(mValidationLayers, "VK_LAYER_KHRONOS_validation");

			InitializeInstance();

			if (IsValidationEnabled())
				InitializeDebugger();
		}

		Device& VulkanInstance::CreateDevice(DeviceFlags flags)
		{
			return *new VulkanDevice(*this, flags);
		}

		void VulkanInstance::DestroyDevice(Device& device)
		{
			device.Terminate();
			delete& device;
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
			glfwSetErrorCallback(_Helpers::GLFWErrorCallback);
		}

		void VulkanInstance::TerminateGLFW()
		{
			glfwTerminate();
		}

		void VulkanInstance::InitializeInstance()
		{
			// Check if the validation layers are supported.
			if (mEnableValidation && !_Helpers::CheckValidationLayerSupport(mValidationLayers))
				FLINT_THROW_RUNTIME_ERROR("Validation layers requested but not available!");

			// Application info.
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Flint Engine";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "Flint";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_2;

			// FInstance create info.
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			// Get and insert the required instance extensions.
			std::vector<const char*> requiredExtensions = std::move(_Helpers::GetRequiredInstanceExtensions(mEnableValidation));
			createInfo.enabledExtensionCount = static_cast<UI32>(requiredExtensions.size());
			createInfo.ppEnabledExtensionNames = requiredExtensions.data();

			// Initialize debugger.
			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
			if (mEnableValidation)
			{
				createInfo.enabledLayerCount = static_cast<UI32>(mValidationLayers.size());
				createInfo.ppEnabledLayerNames = mValidationLayers.data();

				debugCreateInfo = _Helpers::CreateDebugMessengerCreateInfo();
				createInfo.pNext = &debugCreateInfo;
			}
			else
			{
				createInfo.enabledLayerCount = 0;
				createInfo.pNext = nullptr;
			}

			// Create the instance handle.
			FLINT_VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &vInstance));
		}

		void VulkanInstance::TerminateInstance()
		{
			vkDestroyInstance(vInstance, nullptr);
		}

		void VulkanInstance::InitializeDebugger()
		{
			VkDebugUtilsMessengerCreateInfoEXT createInfo = _Helpers::CreateDebugMessengerCreateInfo();

			auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vInstance, "vkCreateDebugUtilsMessengerEXT"));
			FLINT_VK_ASSERT(func(vInstance, &createInfo, nullptr, &vDebugMessenger));
		}

		void VulkanInstance::TerminateDebugger()
		{
			auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vInstance, "vkDestroyDebugUtilsMessengerEXT"));

			if (func != nullptr)
				func(vInstance, vDebugMessenger, nullptr);
		}
	}
}