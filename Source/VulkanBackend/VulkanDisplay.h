// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Inputs/InputCenter.h"
#include "Core/Backend/Display.h"

#include "VulkanInstance.h"

#include <GLFW/glfw3.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice;

		/**
		 * Structure containing support details for a Vulkan Swap Chain.
		 */
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities = {};			// Swap Chain capabilities.
			std::vector<VkSurfaceFormatKHR> formats = {};		// Swap Chain formats.
			std::vector<VkPresentModeKHR> presentModes = {};	// Swap Chain present modes.

			/**
			 * Query swap chain support details.
			 *
			 * @param vPhysicalDevice: The physical device to be checked for.
			 * @param vSurface: The surface to be checked with.
			 * @return SwapChainSupportDetails structure.
			 */
			static SwapChainSupportDetails Query(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface);
		};

		class VulkanDisplay final : public Backend::Display<VulkanDisplay, VulkanInstance> {
		public:
			VulkanDisplay() {}
			~VulkanDisplay() {}

			virtual void mInitialize() override final;
			virtual void mUpdate() override final;
			virtual void mTerminate() override final;

		public:
			VkSurfaceKHR GetSurface() const { return vSurface; }
			VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VulkanDevice* pDevice) const;
			SwapChainSupportDetails GetSwapChainSupportDetails(VulkanDevice* pDevice) const;
			UI32 FindSupporterBufferCount(VulkanDevice* pDevice, UI32 count) const;

			void UpdateWindowExtent(I32 width, I32 height);

		private:
			void SetupInputs();
			void SetupCallbacks();

			void CreateSurface();
			void DestroySurface();

		private:
			GLFWwindow* pWindowHandle = nullptr;
			VkSurfaceKHR vSurface = VK_NULL_HANDLE;
		};
	}
}