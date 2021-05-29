// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend.h"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBackend::VulkanBackend(bool enableValidation) : FBackend(enableValidation)
		{
			// Initialize the GLFW library.
			mInstance.InitializeGLFW();

			// Create the vulkan instance.
			mInstance.CreateInstance(bEnableValidation);

			// If needed, create the vulkan debug messenger.
			if (bEnableValidation)
				mInstance.CreateDebugMessenger();
		}
		
		void VulkanBackend::Terminate()
		{
			// Destroy available devices.
			for (auto& entry : mDevices)
				entry.second.Terminate();

			mDevices.clear();

			// If created, destroy the debug messenger.
			if (bEnableValidation)
				mInstance.DestroyDebugMessenger();

			// Destroy the instance.
			mInstance.DestroyInstance();

			// Terminate GLFW.
			mInstance.TerminateGLFW();
		}

		FDeviceHandle VulkanBackend::CreateDevice()
		{
			mDevices[mDeviceIndex] = VulkanDevice(&mInstance);
			return FDeviceHandle(this, mDeviceIndex++);
		}

		void VulkanBackend::Terminate(FDeviceHandle& handle)
		{
			mDevices[handle].Terminate();
			mDevices.erase(handle);
			handle = {};
		}

		FDisplayHandle VulkanBackend::CreateDisplay(const UI32 width, const UI32 height, const std::string& title)
		{
			mDisplays[mDisplayIndex] = VulkanDisplay(&mInstance, Vector2(width, height), title);
			return FDisplayHandle(this, mDisplayIndex++);
		}

		void VulkanBackend::UpdateDisplay(const FDisplayHandle& handle)
		{
			mDisplays[handle].Update();
		}

		void VulkanBackend::Terminate(FDisplayHandle& handle)
		{
			mDisplays[handle].Terminate();
			mDisplays.erase(handle);
			handle = {};
		}
	}
}