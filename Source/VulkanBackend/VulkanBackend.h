// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/FBackend.h"

#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanDisplay.h"
#include "VulkanBuffer.h"
#include "VulkanRenderTarget.h"

#include <unordered_map>

namespace Flint
{
	namespace VulkanBackend
	{
		class FLINT_API VulkanBackend final : public FBackend
		{
		public:
			VulkanBackend(bool enableValidation);

			virtual void Terminate() override final;

			/** Device functions. */
		public:
			virtual FDeviceHandle CreateDevice() override final;
			virtual void DestroyDevice(FDeviceHandle& handle) override final;
			virtual bool CheckDeviceAndDisplayCompatibility(const FDeviceHandle& deviceHandle, const FDisplayHandle& displayHandle) override final;

			/** Display functions. */
		public:
			virtual FDisplayHandle CreateDisplay(const UI32 width, const UI32 height, const std::string& title) override final;
			virtual void UpdateDisplay(const FDisplayHandle& handle) override final;
			virtual void DestroyDisplay(FDisplayHandle& handle) override final;

			/** Buffer functions. */
		public:
			virtual FBufferHandle CreatBuffer(const FDeviceHandle& deviceHandle, UI64 size, BufferUsage usage, MemoryProfile memoryProfile) override final;
			virtual void* MapBufferMemory(const FBufferHandle& buffer, UI64 size, UI64 offset) override final;
			virtual void UnmapBufferMemory(const FBufferHandle& buffer) override final;
			virtual void DestroyBuffer(FBufferHandle& handle) override final;

			/** Render Target functions. */
		public:
			virtual FRenderTargetHandle CreateRenderTarget(const FDeviceHandle& deviceHandle, const std::vector<std::unique_ptr<FRenderTargetAttachment>>& pAttachments, const FExtent2D& extent, UI32 bufferCount = 0) override final;
			virtual void DestroyRenderTarget(FRenderTargetHandle& handle) override final;

		private:
			VulkanInstance mInstance = {};

			std::unordered_map<UI8, VulkanDevice> mDevices;
			std::unordered_map<UI8, VulkanDisplay> mDisplays;
			std::unordered_map<UI32, VulkanBuffer> mBuffers;
			std::unordered_map<UI16, VulkanRenderTarget> mRenderTargets;

			UI32 mBufferIndex = 0;
			UI16 mRenderTargetIndex = 0;
			UI8 mDisplayIndex = 0;
			UI8 mDeviceIndex = 0;
		};
	}
}