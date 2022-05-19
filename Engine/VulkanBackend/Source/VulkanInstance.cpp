// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanInstance.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanEngine.hpp"

#include <sstream>

#include <spdlog/spdlog.h>

#if defined(FLINT_PLATFORM_WINDOWS)
#include <vulkan/vulkan_win32.h>

#endif

namespace /* anonymous */
{
	/**
	 * Check if the requested validation layers are available.
	 *
	 * @param layers The validation layers to check.
	 * @return Whether or not the validation layers are supported.
	 */
	bool CheckValidationLayerSupport(const std::vector<const char*>& layers)
	{
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Iterate through the layers and check if our layer exists.
		for (const char* layerName : layers)
		{
			bool layerFound = false;
			for (const VkLayerProperties& layerProperties : availableLayers)
			{
				// If the layer exists, we can mark the boolean as true.
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

	/**
	 * Get all the required instance extensions.
	 *
	 * @param enableValidation Whether validation needs to be enabled.
	 * @return The instance extensions.
	 */
	std::vector<const char*> GetRequiredInstanceExtensions(bool enableValidation)
	{
		std::vector<const char*> extensions = { VK_KHR_SURFACE_EXTENSION_NAME , VK_KHR_DISPLAY_EXTENSION_NAME };

#if defined(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);

#elif defined(VK_FUCHSIA_IMAGEPIPE_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_FUCHSIA_IMAGEPIPE_SURFACE_EXTENSION_NAME);

#elif defined(VK_FUCHSIA_IMAGEPIPE_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_FUCHSIA_IMAGEPIPE_SURFACE_EXTENSION_NAME);

#elif defined(VK_MVK_IOS_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);

#elif defined(VK_MVK_MACOS_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);

#elif defined(VK_EXT_METAL_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);

#elif defined(VK_NN_VI_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_NN_VI_SURFACE_EXTENSION_NAME);

#elif defined(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);

#elif defined(FLINT_PLATFORM_WINDOWS)
		extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

#elif defined(VK_KHR_XCB_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);

#elif defined(VK_KHR_XLIB_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);

#elif defined(VK_EXT_DIRECTFB_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_EXT_DIRECTFB_SURFACE_EXTENSION_NAME);

#elif defined(VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME)
		extensions.emplace_back(VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME);

#elif defined(VK_GGP_STREAM_DESCRIPTOR_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_GGP_STREAM_DESCRIPTOR_SURFACE_EXTENSION_NAME);

#elif defined(VK_QNX_SCREEN_SURFACE_EXTENSION_NAME)
		extensions.emplace_back(VK_QNX_SCREEN_SURFACE_EXTENSION_NAME);

#endif

		// If validation is enabled, we need the following extension.
		if (enableValidation)
			extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	/**
	 * Vulkan debug callback.
	 * This function is used by Vulkan to report any internal message to the user.
	 *
	 * @param messageSeverity The severity of the message.
	 * @param messageType The type of the message.
	 * @param pCallbackData The data passed by the API.
	 * @param The user data submitted to Vulkan before this call.
	 * @return The status return.
	 */
	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::stringstream messageStream;
		messageStream << "Vulkan Validation Layer " << ": ";

		if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
			messageStream << "GENERAL | ";

		else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
			messageStream << "VALIDATION | ";

		else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
			messageStream << "PERFORMANCE | ";

		messageStream << pCallbackData->pMessage;

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			spdlog::warn(messageStream.str());
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			spdlog::error(messageStream.str());
			break;

		default:
			spdlog::info(messageStream.str());
			break;
		}

		return VK_FALSE;
	}

	/**
	 * Create the default debug messenger create info structure.
	 *
	 * @return The created structure.
	 */
	VkDebugUtilsMessengerCreateInfoEXT CreateDebugMessengerCreateInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.pNext = VK_NULL_HANDLE;
		createInfo.pUserData = VK_NULL_HANDLE;
		createInfo.flags = 0;
		createInfo.pfnUserCallback = DebugCallback;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		return createInfo;
	}

	/**
	 * Static initializer struct.
	 * These structs are used to initialize data that are to be initialized just once in the application.
	 */
	struct StaticInitializer
	{
		/**
		 * Default constructor.
		 */
		StaticInitializer()
		{
			// Initialize volk.
			FLINT_VK_ASSERT(volkInitialize(), "Failed to initialize volk!");
		}
	};
}

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanInstance::VulkanInstance(std::string&& applicationName, uint32_t applicationVersion, bool enableValidation)
			: Instance(std::move(applicationName), applicationVersion, BackendAPI::Vulkan, enableValidation)
		{
			// Construct the static initializer.
			static StaticInitializer initializer;

			// Create the instance.
			createInstance();

			// Create the debugger if needed.
			if (isValidationEnabled())
				createDebugger();
		}

		VulkanInstance::~VulkanInstance()
		{
			// Destroy the debugger if necessary.
			if (isValidationEnabled())
				destroyDebugger();

			// Destroy the instance.
			destroyInstance();
		}

		std::unique_ptr<Flint::Engine> VulkanInstance::createEngine()
		{
			return std::make_unique<VulkanEngine>(*this);
		}

		void VulkanInstance::createInstance()
		{
			// Setup the application information.
			VkApplicationInfo applicationInfo = {};
			applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationInfo.pNext = nullptr;
			applicationInfo.applicationVersion = getApplicationVersion();
			applicationInfo.pApplicationName = getApplicationName().data();
			applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.pEngineName = "FlintEngine";
			applicationInfo.apiVersion = VulkanVersion;

			// Setup the instance create info.
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &applicationInfo;

			const auto requiredExtensions = GetRequiredInstanceExtensions(isValidationEnabled());
			createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
			createInfo.ppEnabledExtensionNames = requiredExtensions.data();

			VkDebugUtilsMessengerCreateInfoEXT vDebugCreateInfo = {};
			if (isValidationEnabled())
			{
				// Emplace the required validation layer.
				m_ValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");

				// Create the debug messenger create info structure.
				vDebugCreateInfo = CreateDebugMessengerCreateInfo();

				// Submit it to the instance.
				createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
				createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
				createInfo.pNext = &vDebugCreateInfo;
			}
			else
			{
				createInfo.enabledLayerCount = 0;
				createInfo.pNext = nullptr;
			}

			// Create the instance.
			FLINT_VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_Instance), "Failed to create the instance!");

			// Load the instance functions.
			volkLoadInstance(m_Instance);
		}

		void VulkanInstance::destroyInstance()
		{
			vkDestroyInstance(m_Instance, nullptr);
		}

		void VulkanInstance::createDebugger()
		{
			const auto debugMessengerCreateInfo = CreateDebugMessengerCreateInfo();
			const auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
			FLINT_VK_ASSERT(vkCreateDebugUtilsMessengerEXT(m_Instance, &debugMessengerCreateInfo, nullptr, &m_DebugMessenger), "Failed to create the debug messenger.");
		}

		void VulkanInstance::destroyDebugger()
		{
			const auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
			vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		}
	}
}