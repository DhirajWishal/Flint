// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Display.hpp"

#include "VulkanInstance.hpp"
#include <GLFW/glfw3.h>

#include <unordered_map>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice;

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR mCapabilities = {};
			std::vector<VkSurfaceFormatKHR> mFormats = {};
			std::vector<VkPresentModeKHR> mPresentModes = {};

			static SwapChainSupportDetails Query(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface);
		};

		class VulkanDisplay final : public Display
		{
		public:
			VulkanDisplay(Instance& instance, const FExtent2D& extent, const std::string& title);

			virtual void Update() override final;
			virtual void Terminate() override final;
			virtual void ToggleResize() override final { mIsDislayResized = false; }
			virtual UI32 FindBestBufferCount(Device& device, UI32 count = 0) override final;

			VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, UI32 width, UI32 height);
			VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VulkanDevice& device) const;
			UI32 FindSupporterBufferCount(VulkanDevice& device, UI32 count) const;

		public:
			void CallKeyCallback(I32 scancode, I32 action, I32 mods) { if (pKeyCallback) pKeyCallback(mKeyMap[scancode], mActionMap[action], static_cast<SpecialCharacter>(mods)); }
			void CallMouseButtonCallback(I32 button, I32 action, I32 mods) { if (pMouseButtonCallback) pMouseButtonCallback(mButtonMap[button], mActionMap[action], static_cast<SpecialCharacter>(mods)); }
			void CallCursorPositionCallback(double xOffset, double yOffset) { if (pCursorPositionCallback) pCursorPositionCallback(xOffset, yOffset); }
			void CallMouseScrollCallback(double xOffset, double yOffset) { if (pMouseScrollCallback) pMouseScrollCallback(xOffset, yOffset); }
			void CallCursorWithinDisplayCallback(bool state) { if (pCursorWithinDisplayCallback) pCursorWithinDisplayCallback(state); }
			void CallDragAndDropCallback(std::vector<std::filesystem::path> paths) { if (pDragAndDropCallback) pDragAndDropCallback(paths); }
			void CallDisplayResizeCallback(FExtent2D extent) { if (pDisplayResizeCallback) pDisplayResizeCallback(extent); }
			void CallDisplayCloseCallback() { if (pDisplayCloseCallback) pDisplayCloseCallback(); }

			void SetNewExtent(FExtent2D extent) { mExtent = extent; }
			void ToggleClose() { mIsDisplayOpen = false; }
			void Resized() { mIsDislayResized = true; }

		public:
			VkSurfaceKHR GetSurface() const { return vSurface; }

		private:
			void SetupMaps();

		private:
			std::unordered_map<UI32, KeyCode> mKeyMap = {};
			std::vector<MouseButton> mButtonMap = {};
			std::vector<EventAction> mActionMap = {};

			GLFWwindow* pWindow = nullptr;
			VkSurfaceKHR vSurface = VK_NULL_HANDLE;
		};
	}
}
