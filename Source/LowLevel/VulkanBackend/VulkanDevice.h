// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDisplay.h"
#include "VulkanQueue.h"
#include "Core/Backend/Interface/Device.h"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Structure containing support details for a Vulkan Swap Chain.
		 */
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities = {};			// Swap Chain capabilities.
			std::vector<VkSurfaceFormatKHR> formats = {};		// Swap Chain formats.
			std::vector<VkPresentModeKHR> presentModes = {};	// Swap Chain present modes.

			/**
			 * Query swap chain support details.
			 *
			 * @param vPhysicalDevice: The physical device to be checked for.
			 * @param vSurface: The surface to be checked with.
			 * @return SwapChainSupportDetails structure.
			 */
			static SwapChainSupportDetails Query(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface);
		};

		class VulkanDevice {
		public:
			VulkanDevice() {}
			~VulkanDevice() {}

			virtual void Initialize(VulkanDisplay* pDisplay);
			virtual void Terminate();

		public:
			VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() { return vPhysicalDeviceProperties; }
			VkSurfaceCapabilitiesKHR& GetSurfaceCapabilities() { return vSurfaceCapabilities; }
			SwapChainSupportDetails& GetSwapChainSupportDetails() { return vSwapChainSupportDetails; }

			VulkanQueue& GetQueue() { return vQueue; }

			VkQueue GetGraphcisQueue() const { return vQueue.vGraphicsQueue; }
			VkQueue GetComputeQueue() const { return vQueue.vComputeQueue; }
			VkQueue GetTransferQueue() const { return vQueue.vTransferQueue; }

			VkPhysicalDevice GetPhysicalDevice() const { return vPhysicalDevice; }
			VkDevice GetLogicalDevice() const { return vLogicalDevice; }

			VolkDeviceTable* GetDeviceTable() const { return const_cast<VolkDeviceTable*>(&mTable); }

			VulkanDisplay* GetDisplay() const { return pDisplay; }

			VkSampleCountFlags GetSampleCount() const { return vSampleCount; }

			UI32 FindSupporterBufferCount(UI32 count) const;

		private:
			void CreatePhysicalDevice();

			void CreateLogicalDevice();
			void DestroyLogicalDevice();

		private:
			VolkDeviceTable mTable = {};

			VkPhysicalDeviceProperties vPhysicalDeviceProperties = {};
			VkSurfaceCapabilitiesKHR vSurfaceCapabilities = {};
			SwapChainSupportDetails vSwapChainSupportDetails = {};

			VulkanQueue vQueue = {};
			std::vector<const char*> mDeviceExtensions;

			VulkanDisplay* pDisplay = nullptr;

			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;
			VkDevice vLogicalDevice = VK_NULL_HANDLE;

			VkSampleCountFlags vSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;

		public:
			void FreeMemory(VkDeviceMemory vDeviceMemory) const;

			VkResult SubmitQueue(VkQueue vQueue, const std::vector<VkSubmitInfo>& vSubmitInfos, VkFence vFence) const;
			VkResult QueueWait(VkQueue vQueue) const;
			VkResult WaitIdle() const;

			VkResult CreateSwapChain(VkSwapchainCreateInfoKHR* pCreateInfo, VkSwapchainKHR* pSwapChain) const;
			VkResult GetSwapChainImages(VkSwapchainKHR vSwapChain, UI32* pSwapChainImageCount, std::vector<VkImage>& vImages) const;
			void DestroySwapChain(VkSwapchainKHR vSwapChain) const;

			VkResult CreateImage(const VkImageCreateInfo* pCreateInfo, VkImage* pImage) const;
			VkResult CreateImageMemory(std::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const;
			void DestroyImage(VkImage vImage) const;

			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const;
			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const;

			VkResult CreateImageView(const VkImageViewCreateInfo* pCreateInfo, VkImageView* pImageView) const;
			void DestroyImageView(VkImageView vImageView) const;

			VkResult CreateCommanPool(VkCommandPoolCreateInfo* pCreateInfo, VkCommandPool* pPool) const;
			VkResult AllocateCommandBuffers(VkCommandBufferAllocateInfo* pAllicateInfo, const std::vector<VkCommandBuffer>& commandBuffers) const;
			
			VkResult BeginCommandBuffer(VkCommandBuffer vCommandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) const;
			VkResult EndCommandBuffer(VkCommandBuffer vCommandBuffer) const;
			VkResult ResetCommandBuffer(VkCommandBuffer vCommandBuffer, VkCommandBufferResetFlags vResetFlags) const;

			void FreeComandBuffers(VkCommandPool vCommandPool, const std::vector<VkCommandBuffer>& vCommandBuffers) const;
			void DestroyCommandPool(VkCommandPool vCommandPool) const;

			VkResult CreateRenderPass(const VkRenderPassCreateInfo* pCreateInfo, VkRenderPass* pRenderPass) const;
			void DestroyRenderPass(VkRenderPass vRenderPass) const;

			VkResult CreateFrameBuffer(const VkFramebufferCreateInfo* pCreateInfo, VkFramebuffer* pFrameBuffer) const;
			void DestroyFrameBuffers(const std::vector<VkFramebuffer>& vFrameBuffers) const;
		};

		Interface::DeviceHandle CreateDevice(const Interface::DisplayHandle& displayHandle);
		void DestroyDevice(const Interface::DeviceHandle& handle);

		class VulkanDeviceBoundObject {
		public:
			VulkanDeviceBoundObject() {}

			virtual void Terminate() {}

		protected:
			VkDevice GetLogicalDevice() const { return pDevice->GetLogicalDevice(); }
			VkPhysicalDevice GetPhysicalDevice() const { return pDevice->GetPhysicalDevice(); }

		protected:
			VulkanDevice* pDevice = nullptr;
		};
	}
}