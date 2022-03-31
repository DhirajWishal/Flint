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
				auto m_vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				m_vDisplay->ActivateKey(scancode, action, static_cast<SpecialCharacter>(mods));
			}

			void TextCallback(GLFWwindow* window, uint32_t codepoint)
			{
			}

			void CursorPositionCallback(GLFWwindow* window, double xOffset, double yOffset)
			{
				auto m_vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				m_vDisplay->SetMousePosition(static_cast<float>(xOffset), static_cast<float>(yOffset));
			}

			void MouseButtonCallback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
			{
				auto m_vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				m_vDisplay->ActivateMouseButton(button, action, static_cast<SpecialCharacter>(mods));
			}

			void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
			{
				auto m_vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				m_vDisplay->SetMouseScroll(static_cast<float>(xOffset), static_cast<float>(yOffset));
			}

			void MouseCursorEnterCallback(GLFWwindow* window, int32_t entered)
			{
				auto m_vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				m_vDisplay->SetCursorWithinDisplay(entered == GLFW_TRUE);
			}

			void ApplicationDropPathCallback(GLFWwindow* window, int32_t count, const char** strings)
			{
				std::vector<std::filesystem::path> paths(count);
				for (uint32_t i = 0; i < static_cast<uint32_t>(count); i++)
					paths[i] = strings[i];

				auto m_vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				m_vDisplay->SetDragAndDropPaths(std::move(paths));
			}

			void ApplicationResizeCallback(GLFWwindow* window, int32_t width, int32_t height)
			{
				auto m_vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				m_vDisplay->SetNewExtent(FBox2D(width, height));
				m_vDisplay->Resized();
			}

			void WindowCloseCallback(GLFWwindow* window)
			{
				auto m_vDisplay = static_cast<VulkanDisplay*>(glfwGetWindowUserPointer(window));
				m_vDisplay->ToggleClose();
			}
		}

		SwapChainSupportDetails SwapChainSupportDetails::Query(VkPhysicalDevice m_vPhysicalDevice, VkSurfaceKHR m_vSurface)
		{
			SwapChainSupportDetails supportDetails = {};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vPhysicalDevice, m_vSurface, &supportDetails.m_Capabilities);

			uint32_t formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(m_vPhysicalDevice, m_vSurface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				supportDetails.m_Formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(m_vPhysicalDevice, m_vSurface, &formatCount, supportDetails.m_Formats.data());
			}

			uint32_t presentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_vPhysicalDevice, m_vSurface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				supportDetails.m_PresentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(m_vPhysicalDevice, m_vSurface, &presentModeCount, supportDetails.m_PresentModes.data());
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
				if (extent.m_Width > static_cast<uint32_t>(pMode->width) || extent.m_Height > static_cast<uint32_t>(pMode->height))
				{
					pWindow = glfwCreateWindow(1280, 720, title.c_str(), nullptr, nullptr);
					glfwMaximizeWindow(pWindow);

					int32_t width = 0, height = 0;
					glfwGetWindowSize(pWindow, &width, &height);

					m_Extent.m_Width = width;
					m_Extent.m_Height = height;
				}
				else
					pWindow = glfwCreateWindow(extent.m_Width, extent.m_Height, title.c_str(), nullptr, nullptr);
			}
			else
			{
				glfwWindowHint(GLFW_RED_BITS, pMode->redBits);
				glfwWindowHint(GLFW_GREEN_BITS, pMode->greenBits);
				glfwWindowHint(GLFW_BLUE_BITS, pMode->blueBits);
				glfwWindowHint(GLFW_REFRESH_RATE, pMode->refreshRate);

				pWindow = glfwCreateWindow(pMode->width, pMode->height, title.c_str(), pMonitor, NULL);
				m_Extent.m_Width = pMode->width;
				m_Extent.m_Height = pMode->height;
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

			FLINT_VK_ASSERT(glfwCreateWindowSurface(pInstance->StaticCast<VulkanInstance>().GetInstance(), pWindow, nullptr, &m_vSurface));

			SetupMaps();
		}

		void VulkanDisplay::Update()
		{
			glfwPollEvents();
		}

		void VulkanDisplay::Terminate()
		{
			vkDestroySurfaceKHR(pInstance->StaticCast<VulkanInstance>().GetInstance(), m_vSurface, nullptr);
			glfwDestroyWindow(pWindow);

			bIsTerminated = true;
		}

		uint32_t VulkanDisplay::FindBestBufferCount(VulkanDevice* m_pDevice, const uint32_t count)
		{
			OPTICK_EVENT();

			auto m_vSurfaceCapabilities = GetSurfaceCapabilities(*m_pDevice);
			if (count == std::numeric_limits<uint32_t>::max())
				return m_vSurfaceCapabilities.maxImageCount - 1;
			else if (count == 0)
			{
				uint32_t bufferCount = m_vSurfaceCapabilities.minImageCount + 1;
				if (m_vSurfaceCapabilities.maxImageCount > 0
					&& bufferCount > m_vSurfaceCapabilities.maxImageCount)
					bufferCount = m_vSurfaceCapabilities.maxImageCount;

				return bufferCount;
			}

			return count;
		}

		PixelFormat VulkanDisplay::GetBestSwapChainFormat(VulkanDevice* m_pDevice)
		{
			SwapChainSupportDetails m_vSupport = SwapChainSupportDetails::Query(m_pDevice->GetPhysicalDevice(), GetSurface());
			VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(m_vSupport.m_Formats);

			return Utilities::GetPixelFormat(surfaceFormat.format);
		}

		void VulkanDisplay::SetTitle(const std::string& title)
		{
			glfwSetWindowTitle(pWindow, title.c_str());
			m_Title = title;
		}

		void VulkanDisplay::SetExtent(FBox2D newExtent)
		{
			if (newExtent.IsZero())
				throw std::runtime_error("Window extent should be grater than 0!");

			glfwSetWindowSize(pWindow, static_cast<int>(newExtent.m_Width), static_cast<int>(newExtent.m_Height));
			m_Extent = newExtent;
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
			VkSurfaceCapabilitiesKHR m_vCapabilities = {};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysicalDevice(), GetSurface(), &m_vCapabilities);
			return m_vCapabilities;
		}

		uint32_t VulkanDisplay::FindSupporterBufferCount(VulkanDevice& device, const uint32_t count) const
		{
			auto m_vSurfaceCapabilities = GetSurfaceCapabilities(device);
			if (count == std::numeric_limits<uint32_t>::max())
				return m_vSurfaceCapabilities.maxImageCount - 1;
			else if (count == 0)
			{
				uint32_t bufferCount = m_vSurfaceCapabilities.minImageCount + 1;
				if (m_vSurfaceCapabilities.maxImageCount > 0
					&& bufferCount > m_vSurfaceCapabilities.maxImageCount)
					bufferCount = m_vSurfaceCapabilities.maxImageCount;

				return bufferCount;
			}

			return count;
		}

		void VulkanDisplay::ActivateKey(uint32_t scanCode, uint32_t action, SpecialCharacter character)
		{
			m_KeyEvents[static_cast<uint8_t>(m_KeyMap[scanCode])].Activate(m_ActionMap[action], character);
		}

		void VulkanDisplay::ActivateMouseButton(uint32_t scanCode, uint32_t action, SpecialCharacter character)
		{
			m_MouseButtonEvents[static_cast<uint8_t>(m_ButtonMap[scanCode])].Activate(m_ActionMap[action], character);
		}

		void VulkanDisplay::SetMousePosition(float x, float y)
		{
			m_MousePositionX = x;
			m_MousePositionY = y;
		}

		void VulkanDisplay::SetMouseScroll(float x, float y)
		{
			m_MouseScrollUp = x;
			m_MouseScrollDown = y;
		}

		void VulkanDisplay::SetCursorWithinDisplay(bool value)
		{
			m_IsCursorWithinDisplay = value;
		}

		void VulkanDisplay::SetDragAndDropPaths(std::vector<std::filesystem::path>&& paths)
		{
			m_DragAndDropPaths = std::move(paths);
		}

		void VulkanDisplay::SetupMaps()
		{
			OPTICK_EVENT();

			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_SPACE)] = KeyCode::Space;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_APOSTROPHE)] = KeyCode::Apostrophe;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_COMMA)] = KeyCode::Comma;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_MINUS)] = KeyCode::Minus;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_PERIOD)] = KeyCode::Preiod;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_SLASH)] = KeyCode::Slash;

			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_0)] = KeyCode::KeyZero;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_1)] = KeyCode::KeyOne;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_2)] = KeyCode::KeyTwo;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_3)] = KeyCode::KeyThree;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_4)] = KeyCode::KeyFour;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_5)] = KeyCode::KeyFive;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_6)] = KeyCode::KeySix;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_7)] = KeyCode::KeySeven;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_8)] = KeyCode::KeyEight;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_9)] = KeyCode::KeyNine;

			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_SEMICOLON)] = KeyCode::Semicolon;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_EQUAL)] = KeyCode::Equal;

			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_A)] = KeyCode::KeyA;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_B)] = KeyCode::KeyB;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_C)] = KeyCode::KeyC;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_D)] = KeyCode::KeyD;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_E)] = KeyCode::KeyE;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F)] = KeyCode::KeyF;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_G)] = KeyCode::KeyG;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_H)] = KeyCode::KeyH;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_I)] = KeyCode::KeyI;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_J)] = KeyCode::KeyJ;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_K)] = KeyCode::KeyK;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_L)] = KeyCode::KeyL;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_M)] = KeyCode::KeyM;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_N)] = KeyCode::KeyN;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_O)] = KeyCode::KeyO;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_P)] = KeyCode::KeyP;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_Q)] = KeyCode::KeyQ;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_R)] = KeyCode::KeyR;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_S)] = KeyCode::KeyS;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_T)] = KeyCode::KeyT;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_U)] = KeyCode::KeyU;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_V)] = KeyCode::KeyV;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_W)] = KeyCode::KeyW;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_X)] = KeyCode::KeyX;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_Y)] = KeyCode::KeyY;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_Z)] = KeyCode::KeyZ;

			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_BRACKET)] = KeyCode::LeftBracket;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_BACKSLASH)] = KeyCode::Backslash;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_BRACKET)] = KeyCode::RightBracket;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_GRAVE_ACCENT)] = KeyCode::GraveAccent;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_WORLD_1)] = KeyCode::WordOne;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_WORLD_2)] = KeyCode::WordTwo;

			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_ESCAPE)] = KeyCode::Escape;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_ENTER)] = KeyCode::Enter;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_TAB)] = KeyCode::Tab;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_BACKSPACE)] = KeyCode::Backspace;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_INSERT)] = KeyCode::Insert;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_DELETE)] = KeyCode::Delete;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT)] = KeyCode::Right;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT)] = KeyCode::Left;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_DOWN)] = KeyCode::Down;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_UP)] = KeyCode::Up;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_PAGE_UP)] = KeyCode::PageUp;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_PAGE_DOWN)] = KeyCode::PageDowm;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_HOME)] = KeyCode::Home;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_CAPS_LOCK)] = KeyCode::CapsLock;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_SCROLL_LOCK)] = KeyCode::ScrollLock;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_NUM_LOCK)] = KeyCode::NumLock;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_PRINT_SCREEN)] = KeyCode::PrintScreen;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_PAUSE)] = KeyCode::Pause;

			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F1)] = KeyCode::F1;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F2)] = KeyCode::F2;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F3)] = KeyCode::F3;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F4)] = KeyCode::F4;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F5)] = KeyCode::F5;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F6)] = KeyCode::F6;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F7)] = KeyCode::F7;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F8)] = KeyCode::F8;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F9)] = KeyCode::F9;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F10)] = KeyCode::F10;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F11)] = KeyCode::F11;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F12)] = KeyCode::F12;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F13)] = KeyCode::F13;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F14)] = KeyCode::F14;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F15)] = KeyCode::F15;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F16)] = KeyCode::F16;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F17)] = KeyCode::F17;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F18)] = KeyCode::F18;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F19)] = KeyCode::F19;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F20)] = KeyCode::F20;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F21)] = KeyCode::F21;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F22)] = KeyCode::F22;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F23)] = KeyCode::F23;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F24)] = KeyCode::F24;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_F25)] = KeyCode::F25;

			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_0)] = KeyCode::KeyPadZero;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_1)] = KeyCode::KeyPadOne;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_2)] = KeyCode::KeyPadTwo;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_3)] = KeyCode::KeyPadThree;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_4)] = KeyCode::KeyPadFour;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_5)] = KeyCode::KeyPadFive;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_6)] = KeyCode::KeyPadSix;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_7)] = KeyCode::KeyPadSeven;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_8)] = KeyCode::KeyPadEight;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_9)] = KeyCode::KeyPadNine;

			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_DECIMAL)] = KeyCode::KeyPadDecimal;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_DIVIDE)] = KeyCode::KeyPadDivide;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_MULTIPLY)] = KeyCode::KeyPadMultiply;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_SUBTRACT)] = KeyCode::KeyPadSubtract;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_ADD)] = KeyCode::KeyPadAdd;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_ENTER)] = KeyCode::KeyPadEnter;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_KP_EQUAL)] = KeyCode::KeyPadEqual;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_SHIFT)] = KeyCode::LeftShift;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_CONTROL)] = KeyCode::LeftControl;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_ALT)] = KeyCode::LeftAlt;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_LEFT_SUPER)] = KeyCode::LeftSuper;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_SHIFT)] = KeyCode::RightShift;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_CONTROL)] = KeyCode::RightControl;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_ALT)] = KeyCode::RightAlt;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_RIGHT_SUPER)] = KeyCode::RightSuper;
			m_KeyMap[glfwGetKeyScancode(GLFW_KEY_MENU)] = KeyCode::Menu;

			m_ButtonMap.resize(12);
			m_ButtonMap[GLFW_MOUSE_BUTTON_1] = MouseButton::ButtonOne;
			m_ButtonMap[GLFW_MOUSE_BUTTON_2] = MouseButton::ButtonTwo;
			m_ButtonMap[GLFW_MOUSE_BUTTON_3] = MouseButton::ButtonThree;
			m_ButtonMap[GLFW_MOUSE_BUTTON_4] = MouseButton::ButtonFour;
			m_ButtonMap[GLFW_MOUSE_BUTTON_5] = MouseButton::ButtonFive;
			m_ButtonMap[GLFW_MOUSE_BUTTON_6] = MouseButton::ButtonSix;
			m_ButtonMap[GLFW_MOUSE_BUTTON_7] = MouseButton::ButtonSeven;
			m_ButtonMap[GLFW_MOUSE_BUTTON_8] = MouseButton::ButtonEight;

			m_ButtonMap[GLFW_MOUSE_BUTTON_LAST] = MouseButton::Last;
			m_ButtonMap[GLFW_MOUSE_BUTTON_LEFT] = MouseButton::Left;
			m_ButtonMap[GLFW_MOUSE_BUTTON_RIGHT] = MouseButton::Right;
			m_ButtonMap[GLFW_MOUSE_BUTTON_MIDDLE] = MouseButton::Middle;

			m_ActionMap.resize(3);
			m_ActionMap[GLFW_RELEASE] = EventAction::Released;
			m_ActionMap[GLFW_PRESS] = EventAction::Pressed;
			m_ActionMap[GLFW_REPEAT] = EventAction::OnRepeat;
		}
	}
}