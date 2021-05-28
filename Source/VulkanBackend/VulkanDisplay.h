// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Templates/Display.h"
#include "Core/Maths/Vector/Vector2.h"
#include "VulkanInstance.h"

#include <GLFW/glfw3.h>

typedef void* GLFWObjectHandle;

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice;

		/**
		 * Structure containing support details for a Vulkan Swap Chain.
		 */
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR mCapabilities = {};			// Swap Chain capabilities.
			std::vector<VkSurfaceFormatKHR> mFormats = {};			// Swap Chain formats.
			std::vector<VkPresentModeKHR> mPresentModes = {};		// Swap Chain present modes.

			/**
			 * Query swap chain support details.
			 *
			 * @param vPhysicalDevice: The physical device to be checked for.
			 * @param vSurface: The surface to be checked with.
			 * @return SwapChainSupportDetails structure.
			 */
			static SwapChainSupportDetails Query(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface);
		};

		class VulkanDisplay final : public Templates::Display
		{
		public:
			VulkanDisplay(VulkanInstance* pInstance, const Vector2 extent, const std::string& title);

			void Terminate();
			virtual void Update() override final;

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
			VulkanInstance* pInstance = nullptr;
			GLFWwindow* pWindowHandle = nullptr;
			VkSurfaceKHR vSurface = VK_NULL_HANDLE;
		};
	}
}