// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanInstance.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

#include "VulkanBackend/VulkanDevice.hpp"
#include "VulkanBackend/VulkanDisplay.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Helpers
		{
			void GLFWErrorCallback(int code, const char* message)
			{
				if (code != GLFW_NO_ERROR)
					throw std::runtime_error(message);
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
				UI32 glfwExtensionCount = 0;
				const char** glfwExtensions = nullptr;
				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
				std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

				if (enableValidation)
					extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

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

				return extensions;
			}

			static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData)
			{
				std::string myMessagePreStatement = ": ";
				static std::ofstream generalOutputFile("VulkanGeneralOutput.txt");

				if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
				{
					myMessagePreStatement = "(General): ";
					generalOutputFile << "Vulkan Validation Layer " << myMessagePreStatement << pCallbackData->pMessage << std::endl;

					return VK_FALSE;
				}
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
				createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				createInfo.pfnUserCallback = DebugCallback;

				createInfo.pNext = VK_NULL_HANDLE;
				createInfo.pUserData = VK_NULL_HANDLE;
				createInfo.flags = 0;

				return createInfo;
			}
		}

		VulkanInstance::VulkanInstance(bool enableValidation) : Instance(enableValidation)
		{
			OPTICK_EVENT();

			FLINT_VK_ASSERT(volkInitialize());
			InitializeGLFW();

			// Check if Vulkan is supported.
			if (glfwVulkanSupported() != GLFW_TRUE)
				throw std::runtime_error("Vulkan is not supported! Make sure that the drivers support Vulkan before using the Vulkan Backend.");

			if (enableValidation)
				mValidationLayers.push_back("VK_LAYER_KHRONOS_validation");

			InitializeInstance();
			volkLoadInstance(vInstance);

			if (IsValidationEnabled())
				InitializeDebugger();
		}

		std::shared_ptr<Device> VulkanInstance::CreateDevice(const DeviceFlags flags)
		{
			return std::make_shared<VulkanDevice>(shared_from_this(), flags);
		}

		std::shared_ptr<Display> VulkanInstance::CreateDisplay(const FBox2D& extent, const std::string& title)
		{
			return std::make_shared<VulkanDisplay>(shared_from_this(), extent, title);
		}

		void VulkanInstance::Terminate()
		{
			if (IsValidationEnabled())
				TerminateDebugger();

			TerminateInstance();
			TerminateGLFW();

			bIsTerminated = true;
		}

		void VulkanInstance::InitializeGLFW()
		{
			glfwInit();
			glfwSetErrorCallback(Helpers::GLFWErrorCallback);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		void VulkanInstance::TerminateGLFW()
		{
			glfwTerminate();
		}

		void VulkanInstance::InitializeInstance()
		{
			OPTICK_EVENT();

			// Check if the validation layers are supported.
			if (mEnableValidation && !Helpers::CheckValidationLayerSupport(mValidationLayers))
				throw std::runtime_error("Validation layers requested but not available!");

			// Application info.
			VkApplicationInfo appInfo = {};
			appInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Flint Engine";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "Flint";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_2;

			// FInstance create info.
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			// Get and insert the required instance extensions.
			std::vector<const char*> requiredExtensions = std::move(Helpers::GetRequiredInstanceExtensions(mEnableValidation));
			createInfo.enabledExtensionCount = static_cast<UI32>(requiredExtensions.size());
			createInfo.ppEnabledExtensionNames = requiredExtensions.data();

			// Initialize debugger.
			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
			if (mEnableValidation)
			{
				createInfo.enabledLayerCount = static_cast<UI32>(mValidationLayers.size());
				createInfo.ppEnabledLayerNames = mValidationLayers.data();

				debugCreateInfo = Helpers::CreateDebugMessengerCreateInfo();
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
			VkDebugUtilsMessengerCreateInfoEXT createInfo = Helpers::CreateDebugMessengerCreateInfo();

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