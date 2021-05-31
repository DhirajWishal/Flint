// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend.h"
#include "Core/ErrorHandler/Logger.h"

#include "Attachments/VulkanSwapChain.h"
#include "Attachments/VulkanColorBuffer.h"
#include "Attachments/VulkanDepthBuffer.h"

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

		FRenderTargetHandle VulkanBackend::CreateRenderTarget(const FDeviceHandle& deviceHandle, const std::vector<std::unique_ptr<FRenderTargetAttachment>>& pAttachments, const FExtent2D& extent, UI32 bufferCount)
		{
			UI8 deviceIndex = static_cast<UI8>(deviceHandle);
			VulkanDevice* pDevice = &mDevices[deviceIndex];
			//bufferCount = pDevice->Find

			VkFormat vTempFormat = VkFormat::VK_FORMAT_UNDEFINED;	// TODO

			std::vector<VulkanRenderTargetAttachment*> pvAttachments;
			for (auto& pAttachment : pAttachments)
			{
				switch (pAttachment->GetType())
				{
				case Flint::RenderTargetAttachmentType::SWAP_CHAIN:
					FSwapChainAttachment* pSwapChainAttachment = static_cast<FSwapChainAttachment*>(pAttachment.get());
					INSERT_INTO_VECTOR(pvAttachments, new VulkanSwapChain(pDevice, &mDisplays[static_cast<UI8>(pSwapChainAttachment->mDisplayHandle)], extent, bufferCount));
					vTempFormat = pvAttachments[pvAttachments.size() - 1]->GetFormat();
					break;

				case Flint::RenderTargetAttachmentType::COLOR_BUFFER:
					INSERT_INTO_VECTOR(pvAttachments, new VulkanColorBuffer(pDevice, extent, bufferCount, vTempFormat));
					break;

				case Flint::RenderTargetAttachmentType::DEPTH_BUFFER:
					INSERT_INTO_VECTOR(pvAttachments, new VulkanDepthBuffer(pDevice, extent, bufferCount));
					break;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid render target attachment type!"));
					return { 0 };
				}
			}

			mRenderTargets[mRenderTargetIndex] = VulkanRenderTarget(pDevice, pvAttachments, extent, bufferCount);
			return FRenderTargetHandle(mRenderTargetIndex++);
		}

		void VulkanBackend::DestroyRenderTarget(FRenderTargetHandle& handle)
		{
			UI16 index = static_cast<UI16>(handle);
			mRenderTargets[index].Terminate();
			mRenderTargets.erase(index);
			handle = {};
		}
	}
}