// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"
#include "VulkanInstance.hpp"
#include "VulkanQueue.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice final : public Device, public std::enable_shared_from_this<VulkanDevice>
		{
		public:
			VulkanDevice(const std::shared_ptr<Instance>& pInstance, const DeviceFlags flags);
			~VulkanDevice() { if (!bIsTerminated) Terminate(); }

			virtual bool IsDisplayCompatible(const std::shared_ptr<Display>& pDisplay) override final;
			virtual MultiSampleCount GetSupportedMultiSampleCount() const override final;

			virtual std::shared_ptr<CommandBufferAllocator> CreateCommandBufferAllocator(const UI32 bufferCount) override final;
			virtual std::shared_ptr<CommandBufferAllocator> CreateSecondaryCommandBufferAllocator(const UI32 bufferCount, const std::shared_ptr<CommandBufferAllocator>& pParentAllocator) override final;

			virtual std::shared_ptr<ScreenBoundRenderTarget> CreateScreenBoundRenderTarget(const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode) override final;
			virtual std::shared_ptr<OffScreenRenderTarget> CreateOffScreenRenderTarget(const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments) override final;

			virtual std::shared_ptr<Buffer> CreateBuffer(const BufferType type, const UI64 size, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic) override final;

			virtual std::shared_ptr<Image> CreateImage(const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const UI8 layers, const UI32 mipLevels, const void* pImageData, const MultiSampleCount sampleCount = MultiSampleCount::One) override final;

			virtual std::shared_ptr<ImageSampler> CreateImageSampler(const ImageSamplerSpecification& specification) override final;

			virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::filesystem::path& path) override final;
			virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::vector<UI32>& code) override final;
			virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::string& code) override final;

			virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(
				const std::string& pipelineName,
				const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
				const std::shared_ptr<Shader>& pVertexShader,
				const std::shared_ptr<Shader>& pTessellationControlShader,
				const std::shared_ptr<Shader>& pTessellationEvaluationShader,
				const std::shared_ptr<Shader>& pGeometryShader,
				const std::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification) override final;
			virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(
				const std::string& pipelineName,
				const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget,
				const std::shared_ptr<Shader>& pVertexShader,
				const std::shared_ptr<Shader>& pTessellationControlShader,
				const std::shared_ptr<Shader>& pTessellationEvaluationShader,
				const std::shared_ptr<Shader>& pGeometryShader,
				const std::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification) override final;
			virtual std::shared_ptr<ComputePipeline> CreateComputePipeline(const std::string& pipelineName, const std::shared_ptr<Shader>& pShader) override final;

			virtual std::shared_ptr<GeometryStore> CreateGeometryStore(const typename TShaderAttributeMap& vertexAttributes, UI64 indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic) override final;

			virtual void SubmitGraphicsCommandBuffers(const std::vector<std::shared_ptr<CommandBuffer>>& pCommandBuffers) override final;
			virtual void SubmitComputeCommandBuffers(const std::vector<std::shared_ptr<CommandBuffer>>& pCommandBuffers) override final;
			virtual void WaitIdle() override final;
			virtual void WaitForQueue() override final;

			virtual void Terminate() override final;

		public:
			VolkDeviceTable GetDeviceTable() const { return mDeviceTable; }
			VkDevice GetLogicalDevice() const noexcept { return vLogicalDevice; }
			VkPhysicalDevice GetPhysicalDevice() const noexcept { return vPhysicalDevice; }

			VulkanQueue& GetQueue() { return vQueue; }
			const VulkanQueue GetQueue() const { return vQueue; }
			VkSampleCountFlags GetSampleCount() const { return vSampleCount; }

			VkResult CreateImageMemory(const std::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const;
			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, const UI32 mipLevels = 1, const UI32 currentLevel = 0) const;
			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, const UI32 mipLevels = 1, const UI32 currentLevel = 0) const;

		private:
			void InitializePhysicalDevice();

			void InitializeLogicalDevice();
			void TerminateLogicalDevice();

		private:
			VolkDeviceTable mDeviceTable = {};
			VulkanQueue vQueue = {};

			std::vector<const char*> mDeviceExtensions;

			VkDevice vLogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;

			VkSampleCountFlags vSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;

			VkFence vSubmitFence = VK_NULL_HANDLE;
		};
	}
}