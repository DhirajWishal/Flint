// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Display.hpp"

#include "VulkanInstance.hpp"
#include <GLFW/glfw3.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDisplay final : public Display
		{
		public:
			VulkanDisplay(Instance& instance, const FExtent2D& extent, const std::string& title);

			virtual void Update() override final;
			virtual void Terminate() override final;

			void CallKeyCallback(I32 scancode, I32 action, I32 mods) { if (pKeyCallback) pKeyCallback(mKeyMap[scancode], mActionMap[action], static_cast<SpecialCharacter>(mods)); }
			void CallMouseButtonCallback(I32 button, I32 action, I32 mods) { if (pMouseButtonCallback) pMouseButtonCallback(mButtonMap[button], mActionMap[action], static_cast<SpecialCharacter>(mods)); }
			void CallCursorPositionCallback(double xOffset, double yOffset) { if (pCursorPositionCallback) pCursorPositionCallback(xOffset, yOffset); }
			void CallMouseScrollCallback(double xOffset, double yOffset) { if (pMouseScrollCallback) pMouseScrollCallback(xOffset, yOffset); }
			void CallCursorWithinDisplayCallback(bool state) { if (pCursorWithinDisplayCallback) pCursorWithinDisplayCallback(state); }
			void CallDragAndDropCallback(std::vector<std::filesystem::path> paths) { if (pDragAndDropCallback) pDragAndDropCallback(paths); }
			void CallDisplayResizeCallback(FExtent2D extent) { if (pDisplayResizeCallback) pDisplayResizeCallback(extent); }
			void CallDisplayCloseCallback() { if (pDisplayCloseCallback) pDisplayCloseCallback(); }

			void SetNewExtent(FExtent2D extent) { mExtent = extent; }
			void ToggleDisplayClose() { mIsDisplayOpen = false; }

		public:
			VkSurfaceKHR GetSurface() const { return vSurface; }

		private:
			void SetupMaps();

		private:
			KeyCode mKeyMap[121] = {};
			MouseButton mButtonMap[12] = {};
			EventAction mActionMap[3] = {};

			GLFWwindow* pWindow = nullptr;
			VkSurfaceKHR vSurface = VK_NULL_HANDLE;
		};
	}
}
