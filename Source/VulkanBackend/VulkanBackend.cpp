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
			// Destroy all available buffers.
			for (auto& entry : mBuffers)
				entry.second.Terminate();
			mBuffers.clear();

			// Destroy all available devices.
			for (auto& entry : mDevices)
				entry.second.Terminate();
			mDevices.clear();

			// Destroy all available displays.
			for (auto& entry : mDisplays)
				entry.second.Terminate();
			mDisplays.clear();

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
			return FDeviceHandle(mDeviceIndex++);
		}

		void VulkanBackend::DestroyDevice(FDeviceHandle& handle)
		{
			UI8 index = static_cast<UI8>(handle);
			mDevices[index].Terminate();
			mDevices.erase(index);
			handle = {};
		}

		bool VulkanBackend::CheckDeviceAndDisplayCompatibility(const FDeviceHandle& deviceHandle, const FDisplayHandle& displayHandle)
		{
			UI8 deviceIndex = static_cast<UI8>(deviceHandle);
			UI8 displayIndex = static_cast<UI8>(displayHandle);
			return mDevices[deviceIndex].CheckDisplayCompatibility(mDisplays[displayIndex]);
		}

		FDisplayHandle VulkanBackend::CreateDisplay(const UI32 width, const UI32 height, const std::string& title)
		{
			mDisplays[mDisplayIndex] = VulkanDisplay(&mInstance, Vector2(width, height), title);
			return FDisplayHandle(mDisplayIndex++);
		}

		void VulkanBackend::UpdateDisplay(const FDisplayHandle& handle)
		{
			UI8 index = static_cast<UI8>(handle);
			mDisplays[index].Update();
		}

		void VulkanBackend::DestroyDisplay(FDisplayHandle& handle)
		{
			UI8 index = static_cast<UI8>(handle);
			mDisplays[index].Terminate();
			mDisplays.erase(index);
			handle = {};
		}

		FBufferHandle VulkanBackend::CreatBuffer(const FDeviceHandle& deviceHandle, UI64 size, BufferUsage usage, MemoryProfile memoryProfile)
		{
			mBuffers[mBufferIndex] = VulkanBuffer(&mDevices[static_cast<UI8>(deviceHandle)], size, usage, memoryProfile);
			return FBufferHandle(mBufferIndex++);
		}

		void* VulkanBackend::MapBufferMemory(const FBufferHandle& buffer, UI64 size, UI64 offset)
		{
			UI32 index = static_cast<UI32>(buffer);
			return mBuffers[index].MapMemory(size, offset);
		}

		void VulkanBackend::UnmapBufferMemory(const FBufferHandle& buffer)
		{
			UI32 index = static_cast<UI32>(buffer);
			mBuffers[index].UnmapMemory();
		}

		void VulkanBackend::DestroyBuffer(FBufferHandle& handle)
		{
			UI32 index = static_cast<UI32>(handle);
			mBuffers[index].Terminate();
			mBuffers.erase(index);
			handle = {};
		}

		FRenderTargetHandle VulkanBackend::CreateScreenBoundRenderTarget(const FDeviceHandle& deviceHandle, const FDisplayHandle& displayHandle, const FExtent2D& extent, UI32 bufferCount)
		{
			return FRenderTargetHandle();
		}

		void VulkanBackend::DestroyRenderTarget(FRenderTargetHandle& handle)
		{
		}
	}
}