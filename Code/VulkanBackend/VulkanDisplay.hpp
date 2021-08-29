// Copyright 2021 Dhiraj Wishal
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
			boost::container::vector<VkSurfaceFormatKHR> mFormats = {};
			boost::container::vector<VkPresentModeKHR> mPresentModes = {};

			static SwapChainSupportDetails Query(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface);
		};

		class VulkanDisplay final : public Display, public boost::enable_shared_from_this<VulkanDisplay>
		{
		public:
			VulkanDisplay(const boost::shared_ptr<Instance>& pInstance, const FBox2D& extent, const std::string& title);
			~VulkanDisplay() { if (!bIsTerminated) Terminate(); }

			virtual void Update() override final;
			virtual void Terminate() override final;
			virtual void ToggleResize() override final { mIsDislayResized = false; }
			virtual UI32 FindBestBufferCount(const boost::shared_ptr<Device>& pDevice, const UI32 count = 0) override final;
			virtual PixelFormat GetBestSwapChainFormat(const boost::shared_ptr<Device>& pDevice) override final;
			virtual void SetTitle(const std::string& title) override final;
			virtual void SetExtent(FBox2D newExtent) override final;

			VkSurfaceFormatKHR ChooseSurfaceFormat(const boost::container::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR ChoosePresentMode(const boost::container::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, UI32 width, UI32 height);
			VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VulkanDevice& device) const;
			UI32 FindSupporterBufferCount(VulkanDevice& device, const UI32 count) const;

			void ActivateKey(UI32 scanCode, UI32 action, SpecialCharacter character);
			void ActivateMouseButton(UI32 scanCode, UI32 action, SpecialCharacter character);
			void SetMousePosition(float x, float y);
			void SetMouseScroll(float x, float y);
			void SetCursorWithinDisplay(bool value);
			void SetDragAndDropPaths(boost::container::vector<boost::filesystem::path>&& paths);

		public:
			void SetNewExtent(FBox2D extent) { mExtent = extent; }
			void ToggleClose() { mIsDisplayOpen = false; }
			void Resized() { mIsDislayResized = true; }

		public:
			VkSurfaceKHR GetSurface() const { return vSurface; }

		private:
			void SetupMaps();

		private:
			boost::unordered::unordered_map<UI32, KeyCode> mKeyMap = {};
			boost::container::vector<MouseButton> mButtonMap = {};
			boost::container::vector<EventAction> mActionMap = {};

			GLFWwindow* pWindow = nullptr;
			VkSurfaceKHR vSurface = VK_NULL_HANDLE;
		};
	}
}