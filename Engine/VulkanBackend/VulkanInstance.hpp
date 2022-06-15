// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Instance.hpp"

#include <vector>

#include <volk.h>

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan API version that we will be using.
		 */
		constexpr auto VulkanVersion = VK_API_VERSION_1_3;

		/**
		 * Vulkan instance class.
		 * This class contains the Vulkan instance and all it's related information.
		 */
		class VulkanInstance final : public Core::Instance
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param applicationName The name of the application.
			 * @param applicationVersion The version of the application.
			 * @param enableValidation Whether or not to enable backend validation.
			 */
			explicit VulkanInstance(std::string&& applicationName, uint32_t applicationVersion, bool enableValidation);

			/**
			 * Destructor.
			 */
			~VulkanInstance() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

			/**
			 * Get all the stored validation layer.
			 *
			 * @retrun The validation layers.
			 */
			[[nodiscard]] const std::vector<const char*>& getValidationLayers() const { return m_ValidationLayers; }

			/**
			 * Get the Vulkan instance from the class.
			 *
			 * @retrun The instance handle.
			 */
			[[nodiscard]] VkInstance getInstance() const { return m_Instance; }

		private:
			/**
			 * Create the Vulkan instance.
			 */
			void createInstance();

			/**
			 * Destroy the Vulkan instance.
			 */
			void destroyInstance();

			/**
			 * Create the debugger.
			 */
			void createDebugger();

			/**
			 * Destroy the debugger.
			 */
			void destroyDebugger();

		private:
			std::vector<const char*> m_ValidationLayers;

			VkInstance m_Instance = VK_NULL_HANDLE;
			VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
		};
	}
}