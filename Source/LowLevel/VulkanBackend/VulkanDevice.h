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

			VkQueue GetGraphcisQueue() const { vQueue.vGraphicsQueue; }
			VkQueue GetComputeQueue() const { vQueue.vComputeQueue; }
			VkQueue GetTransferQueue() const { vQueue.vTransferQueue; }

			VkPhysicalDevice GetPhysicalDevice() const { return vPhysicalDevice; }
			VkDevice GetLogicalDevice() const { return vLogicalDevice; }

			VolkDeviceTable* GetDeviceTable() const { return const_cast<VolkDeviceTable*>(&mTable); }

			VulkanDisplay* GetDisplay() const { return pDisplay; }

			VkSampleCountFlags GetSampleCount() const { return vSampleCount; }

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

			VkResult CreateSwapChain(VkSwapchainCreateInfoKHR* pCreateInfo, VkSwapchainKHR* pSwapChain) const;
			VkResult GetSwapChainImages(VkSwapchainKHR vSwapChain, UI32* pSwapChainImageCount, std::vector<VkImage>& vImages) const;
			void DestroySwapChain(VkSwapchainKHR vSwapChain) const;

			VkResult CreateImage(const VkImageCreateInfo* pCreateInfo, VkImage* pImage) const;
			VkResult CreateImageMemory(std::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const;
			void DestroyImage(VkImage vImage) const;

			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const;

			VkResult CreateImageView(const VkImageViewCreateInfo* pCreateInfo, VkImageView* pImageView) const;
			void DestroyImageView(VkImageView vImageView) const;

			VkResult CreateCommanPool(VkCommandPoolCreateInfo* pCreateInfo, VkCommandPool* pPool) const;
			VkResult AllocateCommandBuffers(VkCommandBufferAllocateInfo* pAllicateInfo, std::vector<VkCommandBuffer>& commandBuffers) const;
			void FreeComandBuffers(VkCommandPool vCommandPool, std::vector<VkCommandBuffer>& vCommandBuffers) const;
			void DestroyCommandPool(VkCommandPool vCommandPool) const;
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