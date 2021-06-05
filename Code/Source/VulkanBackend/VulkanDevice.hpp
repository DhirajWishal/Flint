// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Device.hpp"
#include "VulkanInstance.hpp"
#include "VulkanQueue.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice final : public Device
		{
		public:
			VulkanDevice(Instance& instance, DeviceFlags flags);

			virtual bool IsDisplayCompatible(const Display& display) override final;

			virtual CommandBufferList& CreatePrimaryCommandBufferList(UI32 bufferCount) override final;
			virtual CommandBufferList& CreateSecondaryCommandBufferList(UI32 bufferCount, CommandBufferList& parent) override final;
			virtual void DestroyCommandBufferList(CommandBufferList& commandBufferList) override final;

			virtual ScreenBoundRenderTarget& CreateScreenBoundRenderTarget(Display& display, const FExtent2D& extent, const UI32 bufferCount) override final;
			virtual void DestroyRenderTarget(RenderTarget& renderTarget) override final;

			virtual void WaitIdle() override final;
			virtual void WaitForQueue() override final;

			virtual StaggingBuffer& CreateStaggingBuffer(UI64 size) override final;
			virtual UniformBuffer& CreateUniformBuffer(UI64 size) override final;
			virtual StorageBuffer& CreateStorageBuffer(UI64 size) override final;
			virtual VertexBuffer& CreateVertexBuffer(UI64 size, const VertexDescriptor& descriptor) override final;
			virtual IndexBuffer& CreateIndexBuffer(UI64 size, UI64 stride) override final;
			virtual void DestroyBuffer(Buffer& buffer) override final;

			virtual void Terminate() override final;

		public:
			VkDevice GetLogicalDevice() const noexcept { return vLogicalDevice; }
			VkPhysicalDevice GetPhysicalDevice() const noexcept { return vPhysicalDevice; }

			VulkanQueue& GetQueue() { return vQueue; }
			const VulkanQueue GetQueue() const { return vQueue; }
			VkSampleCountFlags GetSampleCount() const { return vSampleCount; }

			VkResult CreateImageMemory(const std::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const;
			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const;
			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const;

		private:
			void InitializePhysicalDevice();

			void InitializeLogicalDevice();
			void TerminateLogicalDevice();

		private:
			VulkanQueue vQueue = {};

			std::vector<const char*> mDeviceExtensions;

			VkDevice vLogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;

			VkSampleCountFlags vSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;
		};
	}
}