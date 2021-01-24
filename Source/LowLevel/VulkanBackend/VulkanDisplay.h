// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Interface/Display.h"
#include "Core/Inputs/InputCenter.h"

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

			virtual void Initialize(VulkanInstance* pInstance, UI32 width, UI32 height, const char* pTitle);
			virtual void Terminate();

		public:
			VkSurfaceKHR GetSurface() const { return vSurface; }
			Inputs::InputCenter* GetInputCenter() { return &mInputCenter; }

			VulkanInstance* GetInstance() const { return const_cast<VulkanInstance*>(pInstance); }

		private:
			void SetupInputs();
			void SetupCallbacks();

			void CreateSurface();
			void DestroySurface();

		private:
			VulkanInstance* pInstance = nullptr;
			Inputs::InputCenter mInputCenter = {};

			GLFWwindow* pWindowHandle = nullptr;
			VkSurfaceKHR vSurface = VK_NULL_HANDLE;
		};

		Interface::DisplayHandle CreateDisplay(const Interface::InstanceHandle& instanceHandle, UI32 width, UI32 height, const char* pTitle);
		void DestroyDisplay(const Interface::DisplayHandle& handle);
		Inputs::InputCenter* GetInputCenter(const Interface::DisplayHandle& handle);
	}
}