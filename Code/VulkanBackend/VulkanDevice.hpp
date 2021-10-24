// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"
#include "VulkanInstance.hpp"
#include "VulkanQueue.hpp"

#include <vk_mem_alloc.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice final : public Device, public std::enable_shared_from_this<VulkanDevice>
		{
		public:
			VulkanDevice(const std::shared_ptr<Instance>& pInstance, const DeviceFlags flags);
			~VulkanDevice() { if (!bIsTerminated) Terminate(); }

			virtual bool IsDisplayCompatible(const Display* pDisplay) override;
			virtual MultiSampleCount GetSupportedMultiSampleCount() const override;

			virtual std::shared_ptr<CommandBufferAllocator> CreateCommandBufferAllocator(const UI32 bufferCount) override;
			virtual std::shared_ptr<CommandBufferAllocator> CreateSecondaryCommandBufferAllocator(const UI32 bufferCount, const std::shared_ptr<CommandBufferAllocator>& pParentAllocator) override;

			virtual std::shared_ptr<SwapChain> CreateSwapChain(const std::shared_ptr<Display>& pDisplay, UI32 imageCount, const SwapChainPresentMode presentMode) override;

			virtual std::shared_ptr<ScreenBoundRenderTarget> CreateScreenBoundRenderTarget(const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor = FColor4D(0.0f)) override;
			virtual std::shared_ptr<OffScreenRenderTarget> CreateOffScreenRenderTarget(const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments) override;

			virtual std::shared_ptr<Buffer> CreateBuffer(const BufferType type, const UI64 size, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic) override;

			virtual std::shared_ptr<Image> CreateImage(const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const UI8 layers, const UI32 mipLevels, const void* pImageData, const MultiSampleCount sampleCount = MultiSampleCount::One) override;

			virtual std::shared_ptr<ImageSampler> CreateImageSampler(const ImageSamplerSpecification& specification) override;

			virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::filesystem::path& path) override;
			virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::vector<UI32>& code) override;
			virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::string& code) override;

			virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(
				const std::string& pipelineName,
				const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
				const std::shared_ptr<Shader>& pVertexShader,
				const std::shared_ptr<Shader>& pTessellationControlShader,
				const std::shared_ptr<Shader>& pTessellationEvaluationShader,
				const std::shared_ptr<Shader>& pGeometryShader,
				const std::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification) override;
			virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(
				const std::string& pipelineName,
				const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget,
				const std::shared_ptr<Shader>& pVertexShader,
				const std::shared_ptr<Shader>& pTessellationControlShader,
				const std::shared_ptr<Shader>& pTessellationEvaluationShader,
				const std::shared_ptr<Shader>& pGeometryShader,
				const std::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification) override;
			virtual std::shared_ptr<ComputePipeline> CreateComputePipeline(const std::string& pipelineName, const std::shared_ptr<Shader>& pShader) override;

			virtual std::shared_ptr<GeometryStore> CreateGeometryStore(const typename std::vector<ShaderAttribute>& vertexAttributes, UI64 indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic) override;

			virtual std::shared_ptr<HostSynchronizationPrimitive> CreateHostSynchronizationPrimitive() override;
			virtual std::shared_ptr<DeviceSynchronizationPrimitive> CreateDeviceSynchronizationPrimitive() override;
			virtual std::shared_ptr<Query> CreateQuery(const QueryUsage usage, const UI32 queryCount) override;

			virtual void SubmitGraphicsCommandBuffer(const CommandBuffer* pCommandBuffer, SynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void SubmitGraphicsCommandBuffers(const std::vector<CommandBuffer*>& pCommandBuffers, SynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void SubmitComputeCommandBuffer(const CommandBuffer* pCommandBuffer, SynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void SubmitComputeCommandBuffers(const std::vector<CommandBuffer*>& pCommandBuffers, SynchronizationPrimitive* pPrimitive = nullptr) override;
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
			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, const UI32 mipLevels = 1, const UI32 currentLevel = 0, VkImageAspectFlags vAspects = VkImageAspectFlagBits::VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM) const;
			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, const UI32 mipLevels = 1, const UI32 currentLevel = 0, VkImageAspectFlags vAspects = VkImageAspectFlagBits::VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM) const;

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