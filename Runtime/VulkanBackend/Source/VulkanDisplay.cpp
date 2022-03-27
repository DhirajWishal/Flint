// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanDisplay.hpp"
#include "VulkanBackend/VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Callbacks
		{
			void KeyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
			{
				auto vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				vDisplay->ActivateKey(scancode, action, static_cast<SpecialCharacter>(mods));
			}

			void TextCallback(GLFWwindow* window, uint32_t codepoint)
			{
			}

			void CursorPositionCallback(GLFWwindow* window, double xOffset, double yOffset)
			{
				auto vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				vDisplay->SetMousePosition(static_cast<float>(xOffset), static_cast<float>(yOffset));
			}

			void MouseButtonCallback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
			{
				auto vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				vDisplay->ActivateMouseButton(button, action, static_cast<SpecialCharacter>(mods));
			}

			void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
			{
				auto vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				vDisplay->SetMouseScroll(static_cast<float>(xOffset), static_cast<float>(yOffset));
			}

			void MouseCursorEnterCallback(GLFWwindow* window, int32_t entered)
			{
				auto vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				vDisplay->SetCursorWithinDisplay(entered == GLFW_TRUE);
			}

			void ApplicationDropPathCallback(GLFWwindow* window, int32_t count, const char** strings)
			{
				std::vector<std::filesystem::path> paths(count);
				for (uint32_t i = 0; i < static_cast<uint32_t>(count); i++)
					paths[i] = strings[i];

				auto vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				vDisplay->SetDragAndDropPaths(std::move(paths));
			}

			void ApplicationResizeCallback(GLFWwindow* window, int32_t width, int32_t height)
			{
				auto vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				vDisplay->SetNewExtent(FBox2D(width, height));
				vDisplay->Resized();
			}

			void WindowCloseCallback(GLFWwindow* window)
			{
				auto vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				vDisplay->ToggleClose();
			}
		}

		SwapChainSupportDetails SwapChainSupportDetails::Query(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface)
		{
			SwapChainSupportDetails supportDetails = {};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vPhysicalDevice, vSurface, &supportDetails.mCapabilities);

			uint32_t formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, vSurface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				supportDetails.mFormats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, vSurface, &formatCount, supportDetails.mFormats.data());
			}

			uint32_t presentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, vSurface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				supportDetails.mPresentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, vSurface, &presentModeCount, supportDetails.mPresentModes.data());
			}

			return supportDetails;
		}

		VulkanDisplay::VulkanDisplay(VulkanInstance* pInstance, const FBox2D& extent, const std::string& title) : Display(pInstance, extent, title)
		{
			OPTICK_EVENT();

			GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);

			if (!extent.IsZero())
			{
				if (extent.mWidth > static_cast<uint32_t>(pMode->width) || extent.mHeight > static_cast<uint32_t>(pMode->height))
				{
					pWindow = glfwCreateWindow(1280, 720, title.c_str(), nullptr, nullptr);
					glfwMaximizeWindow(pWindow);

					int32_t width = 0, height = 0;
					glfwGetWindowSize(pWindow, &width, &height);

					mExtent.mWidth = width;
					mExtent.mHeight = height;
				}
				else
					pWindow = glfwCreateWindow(extent.mWidth, extent.mHeight, title.c_str(), nullptr, nullptr);
			}
			else
			{
				glfwWindowHint(GLFW_RED_BITS, pMode->redBits);
				glfwWindowHint(GLFW_GREEN_BITS, pMode->greenBits);
				glfwWindowHint(GLFW_BLUE_BITS, pMode->blueBits);
				glfwWindowHint(GLFW_REFRESH_RATE, pMode->refreshRate);

				pWindow = glfwCreateWindow(pMode->width, pMode->height, title.c_str(), pMonitor, NULL);
				mExtent.mWidth = pMode->width;
				mExtent.mHeight = pMode->height;
			}

			if (!pWindow)
				throw std::runtime_error("Failed to create display!");

			glfwSetWindowUserPointer(pWindow, this);
			glfwSetKeyCallback(pWindow, _Callbacks::KeyCallback);
			glfwSetCursorPosCallback(pWindow, _Callbacks::CursorPositionCallback);
			glfwSetMouseButtonCallback(pWindow, _Callbacks::MouseButtonCallback);
			glfwSetScrollCallback(pWindow, _Callbacks::MouseScrollCallback);
			glfwSetCursorEnterCallback(pWindow, _Callbacks::MouseCursorEnterCallback);
			glfwSetDropCallback(pWindow, _Callbacks::ApplicationDropPathCallback);
			glfwSetWindowCloseCallback(pWindow, _Callbacks::WindowCloseCallback);
			glfwSetWindowSizeCallback(pWindow, _Callbacks::ApplicationResizeCallback);

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

			bIsTerminated = true;
		}

		uint32_t VulkanDisplay::FindBestBufferCount(VulkanDevice* pDevice, const uint32_t count)
		{
			OPTICK_EVENT();

			auto vSurfaceCapabilities = GetSurfaceCapabilities(*pDevice);
			if (count == std::numeric_limits<uint32_t>::max())
				return vSurfaceCapabilities.maxImageCount - 1;
			else if (count == 0)
			{
				uint32_t bufferCount = vSurfaceCapabilities.minImageCount + 1;
				if (vSurfaceCapabilities.maxImageCount > 0
					&& bufferCount > vSurfaceCapabilities.maxImageCount)
					bufferCount = vSurfaceCapabilities.maxImageCount;

				return bufferCount;
			}

			return count;
		}

		PixelFormat VulkanDisplay::GetBestSwapChainFormat(VulkanDevice* pDevice)
		{
			SwapChainSupportDetails vSupport = SwapChainSupportDetails::Query(pDevice->GetPhysicalDevice(), GetSurface());
			VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(vSupport.mFormats);

			return Utilities::GetPixelFormat(surfaceFormat.format);
		}

		void VulkanDisplay::SetTitle(const std::string& title)
		{
			glfwSetWindowTitle(pWindow, title.c_str());
			mTitle = title;
		}

		void VulkanDisplay::SetExtent(FBox2D newExtent)
		{
			if (newExtent.IsZero())
				throw std::runtime_error("Window extent should be grater than 0!");

			glfwSetWindowSize(pWindow, static_cast<int>(newExtent.mWidth), static_cast<int>(newExtent.mHeight));
			mExtent = newExtent;
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

		VkExtent2D VulkanDisplay::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
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

		VkSurfaceCapabilitiesKHR VulkanDisplay::GetSurfaceCapabilities(VulkanDevice const& device) const
		{
			VkSurfaceCapabilitiesKHR vCapabilities = {};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysicalDevice(), GetSurface(), &vCapabilities);
			return vCapabilities;
		}

		uint32_t VulkanDisplay::FindSupporterBufferCount(VulkanDevice& device, const uint32_t count) const
		{
			auto vSurfaceCapabilities = GetSurfaceCapabilities(device);
			if (count == std::numeric_limits<uint32_t>::max())
				return vSurfaceCapabilities.maxImageCount - 1;
			else if (count == 0)
			{
				uint32_t bufferCount = vSurfaceCapabilities.minImageCount + 1;
				if (vSurfaceCapabilities.maxImageCount > 0
					&& bufferCount > vSurfaceCapabilities.maxImageCount)
					bufferCount = vSurfaceCapabilities.maxImageCount;

				return bufferCount;
			}

			return count;
		}

		void VulkanDisplay::ActivateKey(uint32_t scanCode, uint32_t action, SpecialCharacter character)
		{
			mKeyEvents[static_cast<uint8_t>(mKeyMap[scanCode])].Activate(mActionMap[action], character);
		}

		void VulkanDisplay::ActivateMouseButton(uint32_t scanCode, uint32_t action, SpecialCharacter character)
		{
			mMouseButtonEvents[static_cast<uint8_t>(mButtonMap[scanCode])].Activate(mActionMap[action], character);
		}

		void VulkanDisplay::SetMousePosition(float x, float y)
		{
			mMousePositionX = x;
			mMousePositionY = y;
		}

		void VulkanDisplay::SetMouseScroll(float x, float y)
		{
			mMouseScrollUp = x;
			mMouseScrollDown = y;
		}

		void VulkanDisplay::SetCursorWithinDisplay(bool value)
		{
			mIsCursorWithinDisplay = value;
		}

		void VulkanDisplay::SetDragAndDropPaths(std::vector<std::filesystem::path>&& paths)
		{
			mDragAndDropPaths = std::move(paths);
		}

		void VulkanDisplay::SetupMaps()
		{
			OPTICK_EVENT();

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_SPACE)] = KeyCode::Space;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_APOSTROPHE)] = KeyCode::Apostrophe;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_COMMA)] = KeyCode::Comma;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_MINUS)] = KeyCode::Minus;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_PERIOD)] = KeyCode::Preiod;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_SLASH)] = KeyCode::Slash;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_0)] = KeyCode::KeyZero;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_1)] = KeyCode::KeyOne;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_2)] = KeyCode::KeyTwo;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_3)] = KeyCode::KeyThree;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_4)] = KeyCode::KeyFour;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_5)] = KeyCode::KeyFive;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_6)] = KeyCode::KeySix;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_7)] = KeyCode::KeySeven;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_8)] = KeyCode::KeyEight;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_9)] = KeyCode::KeyNine;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_SEMICOLON)] = KeyCode::Semicolon;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_EQUAL)] = KeyCode::Equal;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_A)] = KeyCode::KeyA;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_B)] = KeyCode::KeyB;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_C)] = KeyCode::KeyC;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_D)] = KeyCode::KeyD;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_E)] = KeyCode::KeyE;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F)] = KeyCode::KeyF;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_G)] = KeyCode::KeyG;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_H)] = KeyCode::KeyH;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_I)] = KeyCode::KeyI;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_J)] = KeyCode::KeyJ;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_K)] = KeyCode::KeyK;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_L)] = KeyCode::KeyL;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_M)] = KeyCode::KeyM;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_N)] = KeyCode::KeyN;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_O)] = KeyCode::KeyO;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_P)] = KeyCode::KeyP;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_Q)] = KeyCode::KeyQ;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_R)] = KeyCode::KeyR;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_S)] = KeyCode::KeyS;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_T)] = KeyCode::KeyT;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_U)] = KeyCode::KeyU;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_V)] = KeyCode::KeyV;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_W)] = KeyCode::KeyW;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_X)] = KeyCode::KeyX;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_Y)] = KeyCode::KeyY;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_Z)] = KeyCode::KeyZ;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_BRACKET)] = KeyCode::LeftBracket;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_BACKSLASH)] = KeyCode::Backslash;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_BRACKET)] = KeyCode::RightBracket;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_GRAVE_ACCENT)] = KeyCode::GraveAccent;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_WORLD_1)] = KeyCode::WordOne;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_WORLD_2)] = KeyCode::WordTwo;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_ESCAPE)] = KeyCode::Escape;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_ENTER)] = KeyCode::Enter;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_TAB)] = KeyCode::Tab;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_BACKSPACE)] = KeyCode::Backspace;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_INSERT)] = KeyCode::Insert;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_DELETE)] = KeyCode::Delete;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT)] = KeyCode::Right;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT)] = KeyCode::Left;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_DOWN)] = KeyCode::Down;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_UP)] = KeyCode::Up;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_PAGE_UP)] = KeyCode::PageUp;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_PAGE_DOWN)] = KeyCode::PageDowm;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_HOME)] = KeyCode::Home;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_CAPS_LOCK)] = KeyCode::CapsLock;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_SCROLL_LOCK)] = KeyCode::ScrollLock;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_NUM_LOCK)] = KeyCode::NumLock;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_PRINT_SCREEN)] = KeyCode::PrintScreen;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_PAUSE)] = KeyCode::Pause;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F1)] = KeyCode::F1;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F2)] = KeyCode::F2;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F3)] = KeyCode::F3;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F4)] = KeyCode::F4;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F5)] = KeyCode::F5;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F6)] = KeyCode::F6;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F7)] = KeyCode::F7;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F8)] = KeyCode::F8;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F9)] = KeyCode::F9;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F10)] = KeyCode::F10;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F11)] = KeyCode::F11;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F12)] = KeyCode::F12;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F13)] = KeyCode::F13;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F14)] = KeyCode::F14;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F15)] = KeyCode::F15;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F16)] = KeyCode::F16;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F17)] = KeyCode::F17;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F18)] = KeyCode::F18;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F19)] = KeyCode::F19;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F20)] = KeyCode::F20;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F21)] = KeyCode::F21;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F22)] = KeyCode::F22;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F23)] = KeyCode::F23;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F24)] = KeyCode::F24;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_F25)] = KeyCode::F25;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_0)] = KeyCode::KeyPadZero;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_1)] = KeyCode::KeyPadOne;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_2)] = KeyCode::KeyPadTwo;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_3)] = KeyCode::KeyPadThree;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_4)] = KeyCode::KeyPadFour;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_5)] = KeyCode::KeyPadFive;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_6)] = KeyCode::KeyPadSix;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_7)] = KeyCode::KeyPadSeven;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_8)] = KeyCode::KeyPadEight;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_9)] = KeyCode::KeyPadNine;

			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_DECIMAL)] = KeyCode::KeyPadDecimal;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_DIVIDE)] = KeyCode::KeyPadDivide;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_MULTIPLY)] = KeyCode::KeyPadMultiply;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_SUBTRACT)] = KeyCode::KeyPadSubtract;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_ADD)] = KeyCode::KeyPadAdd;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_ENTER)] = KeyCode::KeyPadEnter;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_KP_EQUAL)] = KeyCode::KeyPadEqual;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_SHIFT)] = KeyCode::LeftShift;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_CONTROL)] = KeyCode::LeftControl;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_ALT)] = KeyCode::LeftAlt;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_SUPER)] = KeyCode::LeftSuper;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_SHIFT)] = KeyCode::RightShift;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_CONTROL)] = KeyCode::RightControl;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_ALT)] = KeyCode::RightAlt;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_SUPER)] = KeyCode::RightSuper;
			mKeyMap[glfwGetKeyScancode(GLFW_KEY_MENU)] = KeyCode::Menu;

			mButtonMap.resize(12);
			mButtonMap[GLFW_MOUSE_BUTTON_1] = MouseButton::ButtonOne;
			mButtonMap[GLFW_MOUSE_BUTTON_2] = MouseButton::ButtonTwo;
			mButtonMap[GLFW_MOUSE_BUTTON_3] = MouseButton::ButtonThree;
			mButtonMap[GLFW_MOUSE_BUTTON_4] = MouseButton::ButtonFour;
			mButtonMap[GLFW_MOUSE_BUTTON_5] = MouseButton::ButtonFive;
			mButtonMap[GLFW_MOUSE_BUTTON_6] = MouseButton::ButtonSix;
			mButtonMap[GLFW_MOUSE_BUTTON_7] = MouseButton::ButtonSeven;
			mButtonMap[GLFW_MOUSE_BUTTON_8] = MouseButton::ButtonEight;

			mButtonMap[GLFW_MOUSE_BUTTON_LAST] = MouseButton::Last;
			mButtonMap[GLFW_MOUSE_BUTTON_LEFT] = MouseButton::Left;
			mButtonMap[GLFW_MOUSE_BUTTON_RIGHT] = MouseButton::Right;
			mButtonMap[GLFW_MOUSE_BUTTON_MIDDLE] = MouseButton::Middle;

			mActionMap.resize(3);
			mActionMap[GLFW_RELEASE] = EventAction::Released;
			mActionMap[GLFW_PRESS] = EventAction::Pressed;
			mActionMap[GLFW_REPEAT] = EventAction::OnRepeat;
		}
	}
}