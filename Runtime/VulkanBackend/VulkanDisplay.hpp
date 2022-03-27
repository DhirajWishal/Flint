// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Display.hpp"

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

			virtual void Update() override;
			virtual void Terminate() override;
			virtual void ToggleResize() override final { mIsDislayResized = false; }
			virtual uint32_t FindBestBufferCount(const Device* pDevice, const uint32_t count = 0) override;
			virtual PixelFormat GetBestSwapChainFormat(const Device* pDevice) override;
			virtual void SetTitle(const std::string& title) override;
			virtual void SetExtent(FBox2D newExtent) override;

			VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);
			VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VulkanDevice const& device) const;
			uint32_t FindSupporterBufferCount(VulkanDevice& device, const uint32_t count) const;

			void ActivateKey(uint32_t scanCode, uint32_t action, SpecialCharacter character);
			void ActivateMouseButton(uint32_t scanCode, uint32_t action, SpecialCharacter character);
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
			std::unordered_map<uint32_t, KeyCode> mKeyMap = {};
			std::vector<MouseButton> mButtonMap = {};
			std::vector<EventAction> mActionMap = {};

			GLFWwindow* pWindow = nullptr;
			VkSurfaceKHR vSurface = VK_NULL_HANDLE;
		};
	}
}