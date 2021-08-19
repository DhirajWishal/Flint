// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.hpp"

#include "VulkanInstance.hpp"

#include <unordered_map>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

		class VulkanDisplay final : public Display, public std::enable_shared_from_this<VulkanDisplay>
		{
		public:
			VulkanDisplay(const std::shared_ptr<Instance>& pInstance, const FBox2D& extent, const std::string& title);
			~VulkanDisplay() { if (!bIsTerminated) Terminate(); }

			virtual void Update() override final;
			virtual void Terminate() override final;
			virtual void ToggleResize() override final { mIsDislayResized = false; }
			virtual UI32 FindBestBufferCount(const std::shared_ptr<Device>& pDevice, UI32 count = 0) override final;
			virtual void SetTitle(const std::string& title) override final;
			virtual void SetExtent(FBox2D newExtent) override final;

			VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, UI32 width, UI32 height);
			VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VulkanDevice& device) const;
			UI32 FindSupporterBufferCount(VulkanDevice& device, UI32 count) const;

			void ActivateKey(UI32 scanCode, UI32 action, SpecialCharacter character);
			void ActivateMouseButton(UI32 scanCode, UI32 action, SpecialCharacter character);
			void SetMousePosition(float x, float y);
			void SetMouseScroll(float x, float y);
			void SetCursorWithinDisplay(bool value);
			void SetDragAndDropPaths(std::vector<std::filesystem::path>&& paths);

		public:
			void SetNewExtent(FBox2D extent) { mExtent = extent; }
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