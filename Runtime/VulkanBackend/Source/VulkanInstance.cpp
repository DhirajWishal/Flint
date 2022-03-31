// Copyright 2021-2022 Dhiraj Wishal
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
				uint32_t layerCount = 0;
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
				uint32_t glfwExtensionCount = 0;
				const char** glfwExtensions = nullptr;
				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
				std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

				if (enableValidation)
					extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

				//if (pushDescriptorsSupported)
				//	extentions.emplace_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
				//
				//if (checkpointsSupported)
				//	extentions.emplace_back(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
				//
				//if (meshShadingSupported)
				//	extentions.emplace_back(VK_NV_MESH_SHADER_EXTENSION_NAME);
				//
				//extentions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

				return extensions;
			}

			static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData)
			{
				std::string_view myMessagePreStatement = ": ";
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
				VkDebugUtilsMessengerCreateInfoEXT m_vCreateInfo = {};
				m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				m_vCreateInfo.pNext = VK_NULL_HANDLE;
				m_vCreateInfo.pUserData = VK_NULL_HANDLE;
				m_vCreateInfo.flags = 0;
				m_vCreateInfo.pfnUserCallback = DebugCallback;

				m_vCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

				m_vCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

				return m_vCreateInfo;
			}
		}

		VulkanInstance::VulkanInstance(bool enableValidation, bool volkInitialized) : Instance(enableValidation)
		{
			OPTICK_EVENT();

			// Initialize volk.
			FLINT_VK_ASSERT(volkInitialize());

			// Initialize GLFW.
			InitializeGLFW();

			// Check if Vulkan is supported.
			if (glfwVulkanSupported() != GLFW_TRUE)
				throw std::runtime_error("Vulkan is not supported! Make sure that the drivers support Vulkan before using the Vulkan Backend.");

			if (enableValidation)
				m_ValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");

			// Initialize the instance.
			InitializeInstance();

			// Instruct volk to load the instance data.
			volkLoadInstance(m_vInstance);

			// Create the debugger if validation is enabled.
			if (IsValidationEnabled())
				InitializeDebugger();
		}

		std::unique_ptr<VulkanDevice> VulkanInstance::CreateDevice(const DeviceFlags flags)
		{
			return std::make_unique<VulkanDevice>(this, flags);
		}

		std::unique_ptr<VulkanDisplay> VulkanInstance::CreateDisplay(const FBox2D& extent, const std::string& title)
		{
			return std::make_unique<VulkanDisplay>(this, extent, title);
		}

		void VulkanInstance::Terminate()
		{
			// Terminate the debugger if enabled.
			if (IsValidationEnabled())
				TerminateDebugger();

			// Terminate the Vulkan instance.
			TerminateInstance();

			// Terminate GLFW.
			TerminateGLFW();

			bIsTerminated = true;
		}

		void VulkanInstance::InitializeGLFW()
		{
			// Initialize GLFW.
			if (glfwInit() == GLFW_FALSE)
				throw backend_error("Failed to initialize GLFW!");

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
			if (m_EnableValidation && !Helpers::CheckValidationLayerSupport(m_ValidationLayers))
				throw std::runtime_error("Validation layers requested but not available!");

			// Application info.
			VkApplicationInfo m_vApplicationInfo = {};
			m_vApplicationInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
			m_vApplicationInfo.pApplicationName = "Flint Engine";
			m_vApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			m_vApplicationInfo.pEngineName = "Flint";
			m_vApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			m_vApplicationInfo.apiVersion = VK_API_VERSION_1_3;

			// FInstance create info.
			VkInstanceCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			m_vCreateInfo.pApplicationInfo = &m_vApplicationInfo;

			// Initialize debugger.
			std::vector<const char*> requiredExtensions;
			VkDebugUtilsMessengerCreateInfoEXT m_vDebugCreateInfo = {};
			if (m_EnableValidation)
			{
				m_vDebugCreateInfo = Helpers::CreateDebugMessengerCreateInfo();

				// Get and insert the required instance extensions.
				requiredExtensions = std::move(Helpers::GetRequiredInstanceExtensions(m_EnableValidation));
				m_vCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
				m_vCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
				m_vCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
				m_vCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
				m_vCreateInfo.pNext = &m_vDebugCreateInfo;
			}
			else
			{
				m_vCreateInfo.enabledLayerCount = 0;
				m_vCreateInfo.pNext = nullptr;
			}

			// Create the instance handle.
			FLINT_VK_ASSERT(vkCreateInstance(&m_vCreateInfo, nullptr, &m_vInstance));
		}

		void VulkanInstance::TerminateInstance()
		{
			vkDestroyInstance(m_vInstance, nullptr);
		}

		void VulkanInstance::InitializeDebugger()
		{
			VkDebugUtilsMessengerCreateInfoEXT m_vCreateInfo = Helpers::CreateDebugMessengerCreateInfo();

			const auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_vInstance, "vkCreateDebugUtilsMessengerEXT"));
			FLINT_VK_ASSERT(vkCreateDebugUtilsMessengerEXT(m_vInstance, &m_vCreateInfo, nullptr, &m_vDebugMessenger));
		}

		void VulkanInstance::TerminateDebugger()
		{
			const auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_vInstance, "vkDestroyDebugUtilsMessengerEXT"));
			vkDestroyDebugUtilsMessengerEXT(m_vInstance, m_vDebugMessenger, nullptr);
		}
	}
}