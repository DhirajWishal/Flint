// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanDisplay.hpp"
#include "Core/Error.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Callbacks
		{
			void KeyCallback(GLFWwindow* window, I32 key, I32 scancode, I32 action, I32 mods)
			{
				static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window))->CallKeyCallback(scancode, action, mods);
			}

			void TextCallback(GLFWwindow* window, UI32 codepoint)
			{
			}

			void CursorPositionCallback(GLFWwindow* window, double xOffset, double yOffset)
			{
				static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window))->CallCursorPositionCallback(xOffset, yOffset);
			}

			void MouseButtonCallback(GLFWwindow* window, I32 button, I32 action, I32 mods)
			{
				static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window))->CallMouseButtonCallback(button, action, mods);
			}

			void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
			{
				static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window))->CallMouseScrollCallback(xOffset, yOffset);
			}

			void MouseCursorEnterCallback(GLFWwindow* window, I32 entered)
			{
				static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window))->CallCursorWithinDisplayCallback(entered == GLFW_TRUE);
			}

			void ApplicationDropPathCallback(GLFWwindow* window, I32 count, const char** strings)
			{
				std::vector<std::filesystem::path> paths(count);
				for (UI32 i = 0; i < static_cast<UI32>(count); i++)
					paths[i] = strings[i];

				static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window))->CallDragAndDropCallback(paths);
			}

			void ApplicationResizeCallback(GLFWwindow* window, I32 width, I32 height)
			{
				auto pDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				pDisplay->CallDisplayResizeCallback(FExtent2D(width, height));
				pDisplay->SetNewExtent(FExtent2D(width, height));
				pDisplay->Resized();
			}

			void WindowCloseCallback(GLFWwindow* window)
			{
				auto pDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				pDisplay->CallDisplayCloseCallback();
				pDisplay->ToggleClose();
			}
		}

		SwapChainSupportDetails SwapChainSupportDetails::Query(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface)
		{
			SwapChainSupportDetails supportDetails = {};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vPhysicalDevice, vSurface, &supportDetails.mCapabilities);

			UI32 formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, vSurface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				supportDetails.mFormats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, vSurface, &formatCount, supportDetails.mFormats.data());
			}

			UI32 presentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, vSurface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				supportDetails.mPresentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, vSurface, &presentModeCount, supportDetails.mPresentModes.data());
			}

			return supportDetails;
		}

		VulkanDisplay::VulkanDisplay(const std::shared_ptr<Backend::Instance>& pInstance, const FExtent2D& extent, const std::string& title) : Display(pInstance, extent, title)
		{
			pWindow = glfwCreateWindow(extent.mWidth, extent.mHeight, title.c_str(), nullptr, nullptr);

			if (!pWindow)
				FLINT_THROW_RUNTIME_ERROR("Failed to create display!");

			glfwSetWindowUserPointer(pWindow, this);
			glfwSetKeyCallback(pWindow, _Callbacks::KeyCallback);
			glfwSetCursorPosCallback(pWindow, _Callbacks::CursorPositionCallback);
			glfwSetMouseButtonCallback(pWindow, _Callbacks::MouseButtonCallback);
			glfwSetScrollCallback(pWindow, _Callbacks::MouseScrollCallback);
			glfwSetCursorEnterCallback(pWindow, _Callbacks::MouseCursorEnterCallback);
			glfwSetDropCallback(pWindow, _Callbacks::ApplicationDropPathCallback);
			glfwSetWindowCloseCallback(pWindow, _Callbacks::WindowCloseCallback);

			FLINT_VK_ASSERT(glfwCreateWindowSurface(pInstance->StaticCast<VulkanInstance>().GetInstance(), pWindow, nullptr, &vSurface));

			SetupMaps();
		}

		void VulkanDisplay::Update()
		{
			glfwPollEvents();
		}

		void VulkanDisplay::Terminate()
		{
			vkDestroySurfaceKHR(pInstance->StaticCast<VulkanInstance>().GetInstance(), vSurface, nullptr);
			glfwDestroyWindow(pWindow);
		}

		UI32 VulkanDisplay::FindBestBufferCount(const std::shared_ptr<Backend::Device>& pDevice, UI32 count)
		{
			auto& vSurfaceCapabilities = GetSurfaceCapabilities(pDevice->StaticCast<VulkanDevice>());
			if (count == std::numeric_limits<UI32>::max())
				return vSurfaceCapabilities.maxImageCount - 1;
			else if (count == 0)
			{
				UI32 bufferCount = vSurfaceCapabilities.minImageCount + 1;
				if (vSurfaceCapabilities.maxImageCount > 0
					&& bufferCount > vSurfaceCapabilities.maxImageCount)
					bufferCount = vSurfaceCapabilities.maxImageCount;

				return bufferCount;
			}

			return count;
		}

		VkSurfaceFormatKHR VulkanDisplay::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
		{
			for (const auto& availableFormat : availableFormats)
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
					&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					return availableFormat;

			return availableFormats[0];
		}

		VkPresentModeKHR VulkanDisplay::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
		{
			VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					return availablePresentMode;
				else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
					bestMode = availablePresentMode;
			}

			return bestMode;
		}

		VkExtent2D VulkanDisplay::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, UI32 width, UI32 height)
		{
			VkExtent2D actualExtent = {
				width,
				height
			};

			if ((width >= capabilities.maxImageExtent.width) || (width <= capabilities.minImageExtent.width))
				actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));

			if ((height >= capabilities.maxImageExtent.height) || (height <= capabilities.minImageExtent.height))
				actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}

		VkSurfaceCapabilitiesKHR VulkanDisplay::GetSurfaceCapabilities(VulkanDevice& device) const
		{
			VkSurfaceCapabilitiesKHR vCapabilities = {};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysicalDevice(), GetSurface(), &vCapabilities);
			return vCapabilities;
		}

		UI32 VulkanDisplay::FindSupporterBufferCount(VulkanDevice& device, UI32 count) const
		{
			auto& vSurfaceCapabilities = GetSurfaceCapabilities(device);
			if (count == std::numeric_limits<UI32>::max())
				return vSurfaceCapabilities.maxImageCount - 1;
			else if (count == 0)
			{
				UI32 bufferCount = vSurfaceCapabilities.minImageCount + 1;
				if (vSurfaceCapabilities.maxImageCount > 0
					&& bufferCount > vSurfaceCapabilities.maxImageCount)
					bufferCount = vSurfaceCapabilities.maxImageCount;

				return bufferCount;
			}

			return count;
		}

		void VulkanDisplay::SetupMaps()
		{
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_SPACE)] = Backend::KeyCode::SPACE;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_APOSTROPHE)] = Backend::KeyCode::APOSTROPHE;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_COMMA)] = Backend::KeyCode::COMMA;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_MINUS)] = Backend::KeyCode::MINUS;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_PERIOD)] = Backend::KeyCode::PERIOD;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_SLASH)] = Backend::KeyCode::SLASH;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_0)] = Backend::KeyCode::KEY_0;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_1)] = Backend::KeyCode::KEY_1;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_2)] = Backend::KeyCode::KEY_2;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_3)] = Backend::KeyCode::KEY_3;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_4)] = Backend::KeyCode::KEY_4;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_5)] = Backend::KeyCode::KEY_5;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_6)] = Backend::KeyCode::KEY_6;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_7)] = Backend::KeyCode::KEY_7;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_8)] = Backend::KeyCode::KEY_8;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_9)] = Backend::KeyCode::KEY_9;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_SEMICOLON)] = Backend::KeyCode::SEMICOLON;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_EQUAL)] = Backend::KeyCode::EQUAL;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_A)] = Backend::KeyCode::KEY_A;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_B)] = Backend::KeyCode::KEY_B;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_C)] = Backend::KeyCode::KEY_C;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_D)] = Backend::KeyCode::KEY_D;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_E)] = Backend::KeyCode::KEY_E;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F)] = Backend::KeyCode::KEY_F;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_G)] = Backend::KeyCode::KEY_G;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_H)] = Backend::KeyCode::KEY_H;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_I)] = Backend::KeyCode::KEY_I;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_J)] = Backend::KeyCode::KEY_J;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_K)] = Backend::KeyCode::KEY_K;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_L)] = Backend::KeyCode::KEY_L;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_M)] = Backend::KeyCode::KEY_M;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_N)] = Backend::KeyCode::KEY_N;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_O)] = Backend::KeyCode::KEY_O;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_P)] = Backend::KeyCode::KEY_P;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_Q)] = Backend::KeyCode::KEY_Q;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_R)] = Backend::KeyCode::KEY_R;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_S)] = Backend::KeyCode::KEY_S;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_T)] = Backend::KeyCode::KEY_T;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_U)] = Backend::KeyCode::KEY_U;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_V)] = Backend::KeyCode::KEY_V;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_W)] = Backend::KeyCode::KEY_W;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_X)] = Backend::KeyCode::KEY_X;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_Y)] = Backend::KeyCode::KEY_Y;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_Z)] = Backend::KeyCode::KEY_Z;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_BRACKET)] = Backend::KeyCode::LEFT_BRACKET;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_BACKSLASH)] = Backend::KeyCode::BACKSLASH;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_BRACKET)] = Backend::KeyCode::RIGHT_BRACKET;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_GRAVE_ACCENT)] = Backend::KeyCode::GRAVE_ACCENT;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_WORLD_1)] = Backend::KeyCode::WORLD_1;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_WORLD_2)] = Backend::KeyCode::WORLD_2;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_ESCAPE)] = Backend::KeyCode::ESCAPE;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_ENTER)] = Backend::KeyCode::ENTER;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_TAB)] = Backend::KeyCode::TAB;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_BACKSPACE)] = Backend::KeyCode::BACKSPACE;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_INSERT)] = Backend::KeyCode::INSERT;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_DELETE)] = Backend::KeyCode::DELETE;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT)] = Backend::KeyCode::RIGHT;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT)] = Backend::KeyCode::LEFT;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_DOWN)] = Backend::KeyCode::DOWN;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_UP)] = Backend::KeyCode::UP;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_PAGE_UP)] = Backend::KeyCode::PAGE_UP;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_PAGE_DOWN)] = Backend::KeyCode::PAGE_DOWN;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_HOME)] = Backend::KeyCode::HOME;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_CAPS_LOCK)] = Backend::KeyCode::CAPS_LOCK;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_SCROLL_LOCK)] = Backend::KeyCode::SCROLL_LOCK;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_NUM_LOCK)] = Backend::KeyCode::NUM_LOCK;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_PRINT_SCREEN)] = Backend::KeyCode::PRINT_SCREEN;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_PAUSE)] = Backend::KeyCode::PAUSE;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F1)] = Backend::KeyCode::F1;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F2)] = Backend::KeyCode::F2;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F3)] = Backend::KeyCode::F3;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F4)] = Backend::KeyCode::F4;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F5)] = Backend::KeyCode::F5;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F6)] = Backend::KeyCode::F6;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F7)] = Backend::KeyCode::F7;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F8)] = Backend::KeyCode::F8;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F9)] = Backend::KeyCode::F9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F10)] = Backend::KeyCode::F10;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F11)] = Backend::KeyCode::F11;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F12)] = Backend::KeyCode::F12;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F13)] = Backend::KeyCode::F13;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F14)] = Backend::KeyCode::F14;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F15)] = Backend::KeyCode::F15;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F16)] = Backend::KeyCode::F16;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F17)] = Backend::KeyCode::F17;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F18)] = Backend::KeyCode::F18;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F19)] = Backend::KeyCode::F19;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F20)] = Backend::KeyCode::F20;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F21)] = Backend::KeyCode::F21;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F22)] = Backend::KeyCode::F22;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F23)] = Backend::KeyCode::F23;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F24)] = Backend::KeyCode::F24;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F25)] = Backend::KeyCode::F25;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_0)] = Backend::KeyCode::KP_0;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_1)] = Backend::KeyCode::KP_1;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_2)] = Backend::KeyCode::KP_2;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_3)] = Backend::KeyCode::KP_3;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_4)] = Backend::KeyCode::KP_4;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_5)] = Backend::KeyCode::KP_5;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_6)] = Backend::KeyCode::KP_6;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_7)] = Backend::KeyCode::KP_7;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_8)] = Backend::KeyCode::KP_8;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_9)] = Backend::KeyCode::KP_9;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_DECIMAL)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_DIVIDE)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_MULTIPLY)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_SUBTRACT)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_ADD)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_ENTER)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_EQUAL)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_SHIFT)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_CONTROL)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_ALT)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_SUPER)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_SHIFT)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_CONTROL)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_ALT)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_SUPER)] = Backend::KeyCode::KP_9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_MENU)] = Backend::KeyCode::KP_9;

			mButtonMap.resize(12);
			mButtonMap[GLFW_MOUSE_BUTTON_1] = Backend::MouseButton::BUTTON_1;
			mButtonMap[GLFW_MOUSE_BUTTON_2] = Backend::MouseButton::BUTTON_2;
			mButtonMap[GLFW_MOUSE_BUTTON_3] = Backend::MouseButton::BUTTON_3;
			mButtonMap[GLFW_MOUSE_BUTTON_4] = Backend::MouseButton::BUTTON_4;
			mButtonMap[GLFW_MOUSE_BUTTON_5] = Backend::MouseButton::BUTTON_5;
			mButtonMap[GLFW_MOUSE_BUTTON_6] = Backend::MouseButton::BUTTON_6;
			mButtonMap[GLFW_MOUSE_BUTTON_7] = Backend::MouseButton::BUTTON_7;
			mButtonMap[GLFW_MOUSE_BUTTON_8] = Backend::MouseButton::BUTTON_8;

			mButtonMap[GLFW_MOUSE_BUTTON_LAST] = Backend::MouseButton::LAST;
			mButtonMap[GLFW_MOUSE_BUTTON_LEFT] = Backend::MouseButton::LEFT;
			mButtonMap[GLFW_MOUSE_BUTTON_RIGHT] = Backend::MouseButton::RIGHT;
			mButtonMap[GLFW_MOUSE_BUTTON_MIDDLE] = Backend::MouseButton::MIDDLE;

			mActionMap.resize(3);
			mActionMap[GLFW_RELEASE] = Backend::EventAction::RELEASED;
			mActionMap[GLFW_PRESS] = Backend::EventAction::PRESSED;
			mActionMap[GLFW_REPEAT] = Backend::EventAction::ON_REPEAT;
		}
	}
}