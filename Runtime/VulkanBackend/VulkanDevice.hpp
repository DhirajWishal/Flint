// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"
#include "VulkanInstance.hpp"
#include "VulkanQueue.hpp"
#include "GraphicsCore/SynchronizationPrimitive.hpp"

#include <vk_mem_alloc.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanInstance; 
		class VulkanDisplay;
		class VulkanCommandBufferAllocator; 
		class VulkanSwapChain; 
		class VulkanScreenBoundRenderTarget; 
		class VulkanOffScreenRenderTarget; 
		class VulkanBuffer; 
		class VulkanImage;
		class VulkanImageSampler; 
		class VulkanShader; 
		class VulkanGraphicsPipeline; 
		class VulkanComputePipeline; 
		class VulkanGeometryStore;
		class VulkanHostSynchronizationPrimitive;
		class VulkanDeviceSynchronizationPrimitive; 
		class VulkanQuery; 
		class VulkanCommandBuffer;

		class VulkanDevice final : public Device<VulkanDevice, VulkanInstance, VulkanDisplay, VulkanCommandBufferAllocator, VulkanSwapChain, VulkanScreenBoundRenderTarget, VulkanOffScreenRenderTarget, VulkanBuffer, VulkanImage, VulkanImageSampler, VulkanShader, VulkanGraphicsPipeline, VulkanComputePipeline, VulkanGeometryStore, VulkanHostSynchronizationPrimitive, VulkanDeviceSynchronizationPrimitive, VulkanQuery, VulkanCommandBuffer>
		{
		public:
			VulkanDevice(VulkanInstance* pInstance, const DeviceFlags flags);
			~VulkanDevice() { if (!bIsTerminated) Terminate(); }

			virtual bool IsDisplayCompatible(const VulkanDisplay* pDisplay) override;
			virtual MultiSampleCount GetSupportedMultiSampleCount() const override;

			virtual std::unique_ptr<VulkanCommandBufferAllocator> CreateCommandBufferAllocator(const uint32_t bufferCount) override;
			virtual std::unique_ptr<VulkanCommandBufferAllocator> CreateSecondaryCommandBufferAllocator(const uint32_t bufferCount, VulkanCommandBufferAllocator* pParentAllocator) override;

			virtual std::unique_ptr<VulkanSwapChain> CreateSwapChain(VulkanDisplay* pDisplay, uint32_t imageCount, const SwapChainPresentMode presentMode) override;

			virtual std::unique_ptr<VulkanScreenBoundRenderTarget> CreateScreenBoundRenderTarget(VulkanDisplay* pDisplay, const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachment<VulkanImage>>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor = FColor4D(0.0f)) override;
			virtual std::unique_ptr<VulkanOffScreenRenderTarget> CreateOffScreenRenderTarget(const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachment<VulkanImage>>& imageAttachments) override;

			virtual std::unique_ptr<VulkanBuffer> CreateBuffer(const BufferType type, const uint64_t size, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic) override;

			virtual std::unique_ptr<VulkanImage> CreateImage(const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const uint8_t layers, const uint32_t mipLevels, const void* pImageData, const MultiSampleCount sampleCount = MultiSampleCount::One) override;

			virtual std::unique_ptr<VulkanImageSampler> CreateImageSampler(const ImageSamplerSpecification& specification) override;

			virtual std::unique_ptr<VulkanShader> CreateShader(const ShaderType type, const std::filesystem::path& path) override;
			virtual std::unique_ptr<VulkanShader> CreateShader(const ShaderType type, const std::vector<uint32_t>& code) override;
			virtual std::unique_ptr<VulkanShader> CreateShader(const ShaderType type, const std::string& code) override;

			virtual std::unique_ptr<VulkanGraphicsPipeline> CreateGraphicsPipeline(const std::string& pipelineName, VulkanScreenBoundRenderTarget* pScreenBoundRenderTarget, std::unique_ptr<VulkanShader>&& pVertexShader, std::unique_ptr<VulkanShader>&& pTessellationControlShader, std::unique_ptr<VulkanShader>&& pTessellationEvaluationShader, std::unique_ptr<VulkanShader>&& pGeometryShader, std::unique_ptr<VulkanShader>&& pFragmentShader, const GraphicsPipelineSpecification& specification) override;
			virtual std::unique_ptr<VulkanGraphicsPipeline> CreateGraphicsPipeline(const std::string& pipelineName, VulkanOffScreenRenderTarget* pOffScreenRenderTarget, std::unique_ptr<VulkanShader>&& pVertexShader, std::unique_ptr<VulkanShader>&& pTessellationControlShader, std::unique_ptr<VulkanShader>&& pTessellationEvaluationShader, std::unique_ptr<VulkanShader>&& pGeometryShader, std::unique_ptr<VulkanShader>&& pFragmentShader, const GraphicsPipelineSpecification& specification) override;
			virtual std::unique_ptr<VulkanComputePipeline> CreateComputePipeline(const std::string& pipelineName, std::unique_ptr<VulkanShader>&& pShader) override;

			virtual std::shared_ptr<VulkanGeometryStore> CreateGeometryStore(const typename std::vector<ShaderAttribute>& vertexAttributes, uint64_t indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic) override;

			virtual std::unique_ptr<VulkanHostSynchronizationPrimitive> CreateHostSynchronizationPrimitive() override;
			virtual std::unique_ptr<VulkanDeviceSynchronizationPrimitive> CreateDeviceSynchronizationPrimitive() override;
			virtual std::unique_ptr<VulkanQuery> CreateQuery(const QueryUsage usage, const uint32_t queryCount) override;

			virtual void SubmitGraphicsCommandBuffer(const VulkanCommandBuffer* pCommandBuffer, VulkanHostSynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void SubmitGraphicsCommandBuffers(const std::vector<VulkanCommandBuffer*>& pCommandBuffers, VulkanHostSynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void SubmitComputeCommandBuffer(const VulkanCommandBuffer* pCommandBuffer, VulkanHostSynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void SubmitComputeCommandBuffers(const std::vector<VulkanCommandBuffer*>& pCommandBuffers, VulkanHostSynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void WaitIdle() override;
			virtual void WaitForQueue() override;

			virtual void Terminate() override;

		public:
			VolkDeviceTable GetDeviceTable() const { return mDeviceTable; }
			VkDevice GetLogicalDevice() const noexcept { return vLogicalDevice; }
			VkPhysicalDevice GetPhysicalDevice() const noexcept { return vPhysicalDevice; }

			VulkanQueue& GetQueue() { return vQueue; }
			const VulkanQueue GetQueue() const { return vQueue; }
			VkSampleCountFlags GetSampleCount() const { return vSampleCount; }

			VkResult CreateImageMemory(const std::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const;
			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, uint32_t layerCount = 1, uint32_t currentLayer = 0, const uint32_t mipLevels = 1, const uint32_t currentLevel = 0, VkImageAspectFlags vAspects = VkImageAspectFlagBits::VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM) const;
			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, uint32_t layerCount = 1, uint32_t currentLayer = 0, const uint32_t mipLevels = 1, const uint32_t currentLevel = 0, VkImageAspectFlags vAspects = VkImageAspectFlagBits::VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM) const;

			const VmaAllocator GetVmaAllocator() const { return mVmaAllocator; }

		private:
			void InitializePhysicalDevice();

			void InitializeLogicalDevice();
			void TerminateLogicalDevice();

			VmaVulkanFunctions GetVulkanFunctions() const;

			void CreateVmaAllocator();
			void DestroyVmaAllocator() const;

		private:
			VolkDeviceTable mDeviceTable = {};
			VulkanQueue vQueue = {};

			std::vector<const char*> mDeviceExtensions;

			VkDevice vLogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;

			VmaAllocator mVmaAllocator = VK_NULL_HANDLE;

			VkSampleCountFlags vSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;
		};
	}
}