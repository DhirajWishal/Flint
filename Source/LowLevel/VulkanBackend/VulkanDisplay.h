// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Interface/Display.h"
#include "Core/Inputs/InputCenter.h"
#include "Core/Backend/Display.h"

#include "VulkanInstance.h"

#include <GLFW/glfw3.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDisplay : public Backend::Display {
		public:
			VulkanDisplay() {}
			~VulkanDisplay() {}

			virtual void Initialize(Backend::Instance* pInstance, UI32 width, UI32 height, const char* pTitle) override final;
			virtual void Terminate() override final;

		public:
			VkSurfaceKHR GetSurface() const { return vSurface; }

		private:
			void SetupInputs();
			void SetupCallbacks();

			void CreateSurface();
			void DestroySurface();

		private:
			GLFWwindow* pWindowHandle = nullptr;
			VkSurfaceKHR vSurface = VK_NULL_HANDLE;
		};

		Interface::DisplayHandle CreateDisplay(const Interface::InstanceHandle& instanceHandle, UI32 width, UI32 height, const char* pTitle);
		void DestroyDisplay(const Interface::DisplayHandle& handle);
		Inputs::InputCenter* GetInputCenter(const Interface::DisplayHandle& handle);
	}
}