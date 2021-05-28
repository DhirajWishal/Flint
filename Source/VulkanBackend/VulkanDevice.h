// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanInstance.h"
#include "VulkanDisplay.h"
#include "VulkanQueue.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice 
		{
		public:
			VulkanDevice(VulkanInstance* pInstance);

			void Terminate();
			bool CheckDisplayCompatibility(const VulkanDisplay& display);

		public:
			VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;

			VulkanQueue& GetQueue() { return vQueue; }

			VkQueue GetGraphicsQueue() const { return vQueue.vGraphicsQueue; }
			VkQueue GetComputeQueue() const { return vQueue.vComputeQueue; }
			VkQueue GetTransferQueue() const { return vQueue.vTransferQueue; }

			VkPhysicalDevice GetPhysicalDevice() const { return vPhysicalDevice; }
			VkDevice GetLogicalDevice() const { return vLogicalDevice; }

			VkFormatProperties GetFormatProperties(VkFormat format) const;

			VkSampleCountFlags GetSampleCount() const { return vSampleCount; }

		private:
			void CreatePhysicalDevice();

			void CreateLogicalDevice();
			void DestroyLogicalDevice();

		private:
			VulkanQueue vQueue = {};
			std::vector<const char*> mDeviceExtensions;

			VulkanInstance* pInstance = nullptr;

			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;
			VkDevice vLogicalDevice = VK_NULL_HANDLE;

			VkSampleCountFlags vSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;

		public:
			VkResult MapMemory(VkDeviceMemory vDeviceMemory, UI64 size, UI64 offset, void** ppData) const;
			void UnmapMemory(VkDeviceMemory vDeviceMemory) const;
			void FreeMemory(VkDeviceMemory vDeviceMemory) const;
			VkResult FlushMemoryRanges(const std::vector<VkMappedMemoryRange>& vMappedRanges) const;

			VkResult SubmitQueue(VkQueue vQueue, const std::vector<VkSubmitInfo>& vSubmitInfos, VkFence vFence) const;
			VkResult QueueWait(VkQueue vQueue) const;
			VkResult WaitIdle() const;
			VkResult WaitForFences(const std::vector<VkFence>& vFences, bool waitForAll, UI64 timeout) const;
			VkResult ResetFences(const std::vector<VkFence>& vFences) const;

			VkResult CreateSwapChain(VkSwapchainCreateInfoKHR* pCreateInfo, VkSwapchainKHR* pSwapChain) const;
			VkResult GetSwapChainImages(VkSwapchainKHR vSwapChain, UI32* pSwapChainImageCount, const std::vector<VkImage>& vImages) const;
			void DestroySwapChain(VkSwapchainKHR vSwapChain) const;

			VkResult CreateImage(const VkImageCreateInfo* pCreateInfo, VkImage* pImage) const;
			VkResult CreateImageMemory(const std::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const;
			void DestroyImage(VkImage vImage) const;

			VkResult CreateSampler(const VkSamplerCreateInfo* pCreateInfo, VkSampler* pSampler) const;
			void DestroySampler(VkSampler vSampler) const;

			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const;
			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const;

			VkResult CreateImageView(const VkImageViewCreateInfo* pCreateInfo, VkImageView* pImageView) const;
			void DestroyImageView(VkImageView vImageView) const;

			VkResult CreateCommanPool(VkCommandPoolCreateInfo* pCreateInfo, VkCommandPool* pPool) const;
			VkResult AllocateCommandBuffers(VkCommandBufferAllocateInfo* pAllicateInfo, std::vector<VkCommandBuffer>& commandBuffers) const;

			VkResult BeginCommandBuffer(VkCommandBuffer vCommandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) const;
			VkResult EndCommandBuffer(VkCommandBuffer vCommandBuffer) const;
			VkResult ResetCommandBuffer(VkCommandBuffer vCommandBuffer, VkCommandBufferResetFlags vResetFlags) const;

			void FreeComandBuffers(VkCommandPool vCommandPool, const std::vector<VkCommandBuffer>& vCommandBuffers) const;
			void DestroyCommandPool(VkCommandPool vCommandPool) const;

			VkResult CreateRenderPass(const VkRenderPassCreateInfo* pCreateInfo, VkRenderPass* pRenderPass) const;
			void DestroyRenderPass(VkRenderPass vRenderPass) const;

			VkResult CreateFrameBuffer(const VkFramebufferCreateInfo* pCreateInfo, VkFramebuffer* pFrameBuffer) const;
			void DestroyFrameBuffers(const std::vector<VkFramebuffer>& vFrameBuffers) const;

			VkResult CreateBuffer(const VkBufferCreateInfo* pCreateInfo, VkBuffer* pBuffer) const;
			VkResult CreateBufferMemory(const std::vector<VkBuffer>& vBuffers, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const;
			void DestroyBuffer(VkBuffer vBuffer) const;

			VkResult CreateShaderModule(const VkShaderModuleCreateInfo* pCreateInfo, VkShaderModule* pModule) const;
			void DestroyShaderModule(VkShaderModule vModule) const;

			VkResult CreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayout* pLayout) const;
			void DestroyDescriptorSetLayout(VkDescriptorSetLayout vLayout) const;

			VkResult CreateDescriptorPool(const VkDescriptorPoolCreateInfo* pCreateInfo, VkDescriptorPool* pPool) const;
			void DestroyDescriptorPool(VkDescriptorPool vPool) const;

			VkResult AllocateDescriptorSet(const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pSet) const;
			void UpdateDescriptorSet(const std::vector<VkWriteDescriptorSet>& vWrites, const std::vector<VkCopyDescriptorSet>& vCopies) const;

			VkResult CreatePipelineLayout(const VkPipelineLayoutCreateInfo* pCreateInfo, VkPipelineLayout* pLayout) const;
			void DestroyPipelineLayout(VkPipelineLayout vLayout) const;

			VkResult CreateGraphicsPipeline(const VkGraphicsPipelineCreateInfo* pCreateInfo, VkPipeline* pPipeline) const;
			void DestroyPipeline(VkPipeline vPipeline) const;

			VkResult CreateSemaphores(const VkSemaphoreCreateInfo* pCreateInfo, const std::vector<VkSemaphore>& vSemaphores) const;
			void DestroySemaphores(const std::vector<VkSemaphore>& vSemaphores) const;

			VkResult CreateFences(const VkFenceCreateInfo* pCreateInfo, const std::vector<VkFence>& vFences) const;
			void DestroyFences(const std::vector<VkFence>& vFences) const;
		};

		class VulkanDeviceBoundObject
		{
		public:
			VulkanDeviceBoundObject() {}

			virtual void Terminate() {}

		public:
			VulkanDevice* GetDevice() const { return pDevice; }

		protected:
			VkDevice GetLogicalDevice() const { return pDevice->GetLogicalDevice(); }
			VkPhysicalDevice GetPhysicalDevice() const { return pDevice->GetPhysicalDevice(); }

		protected:
			VulkanDevice* pDevice = nullptr;
		};
	}
}