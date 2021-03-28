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
		class VulkanDisplay {
		public:
			VulkanDisplay() {}
			~VulkanDisplay() {}

			void Initialize(VulkanInstance* pInstance, const Vector2 extent, const char* pTitle);
			void Update();
			void Terminate();

		public:
			VkSurfaceKHR GetSurface() const { return vSurface; }
			Vector2 GetExtent() const { return mExtent; }
			Inputs::InputCenter* GetInputCenter() const { return const_cast<Inputs::InputCenter*>(&mInputCenter); }

			void UpdateWindowExtent(I32 width, I32 height);

		private:
			void SetupInputs();
			void SetupCallbacks();

			void CreateSurface();
			void DestroySurface();

		private:
			Inputs::InputCenter mInputCenter = {};

			Vector2 mExtent = Vector2::ZeroAll;
			VulkanInstance* pInstance = nullptr;

			GLFWwindow* pWindowHandle = nullptr;
			VkSurfaceKHR vSurface = VK_NULL_HANDLE;
		};

		Backend::DisplayHandle CreateDisplay(Backend::InstanceHandle instanceHandle, const Vector2 extent, const char* pTitle);
		void UpdateDisplay(Backend::DisplayHandle handle);
		void DestroyDisplay(Backend::DisplayHandle handle);
		Inputs::InputCenter* GetDisplayInputCenter(Backend::DisplayHandle handle);
	}
}