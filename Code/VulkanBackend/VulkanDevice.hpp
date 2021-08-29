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
		class VulkanDevice final : public Device, public boost::enable_shared_from_this<VulkanDevice>
		{
		public:
			VulkanDevice(const boost::shared_ptr<Instance>& pInstance, const DeviceFlags flags);
			~VulkanDevice() { if (!bIsTerminated) Terminate(); }

			virtual bool IsDisplayCompatible(const boost::shared_ptr<Display>& pDisplay) override final;
			virtual MultiSampleCount GetSupportedMultiSampleCount() const override final;

			virtual boost::shared_ptr<CommandBufferAllocator> CreateCommandBufferAllocator(const UI32 bufferCount) override final;
			virtual boost::shared_ptr<CommandBufferAllocator> CreateSecondaryCommandBufferAllocator(const UI32 bufferCount, const boost::shared_ptr<CommandBufferAllocator>& pParentAllocator) override final;

			virtual boost::shared_ptr<ScreenBoundRenderTarget> CreateScreenBoundRenderTarget(const boost::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const boost::container::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode) override final;
			virtual boost::shared_ptr<OffScreenRenderTarget> CreateOffScreenRenderTarget(const FBox2D& extent, const UI32 bufferCount, const boost::container::vector<RenderTargetAttachment>& imageAttachments) override final;

			virtual boost::shared_ptr<Buffer> CreateBuffer(const BufferType type, const UI64 size, const BufferMemoryProfile profile = BufferMemoryProfile::AUTOMATIC) override final;

			virtual boost::shared_ptr<Image> CreateImage(const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const UI8 layers, const UI32 mipLevels, const void* pImageData, const MultiSampleCount sampleCount = MultiSampleCount::BITS_1) override final;

			virtual boost::shared_ptr<ImageSampler> CreateImageSampler(const ImageSamplerSpecification& specification) override final;

			virtual boost::shared_ptr<Shader> CreateShader(const ShaderType type, const boost::filesystem::path& path) override final;
			virtual boost::shared_ptr<Shader> CreateShader(const ShaderType type, const boost::container::vector<UI32>& code) override final;
			virtual boost::shared_ptr<Shader> CreateShader(const ShaderType type, const std::string& code) override final;

			virtual boost::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(
				const std::string& pipelineName,
				const boost::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
				const boost::shared_ptr<Shader>& pVertexShader,
				const boost::shared_ptr<Shader>& pTessellationControlShader,
				const boost::shared_ptr<Shader>& pTessellationEvaluationShader,
				const boost::shared_ptr<Shader>& pGeometryShader,
				const boost::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification) override final;
			virtual boost::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(
				const std::string& pipelineName,
				const boost::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget,
				const boost::shared_ptr<Shader>& pVertexShader,
				const boost::shared_ptr<Shader>& pTessellationControlShader,
				const boost::shared_ptr<Shader>& pTessellationEvaluationShader,
				const boost::shared_ptr<Shader>& pGeometryShader,
				const boost::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification) override final;
			virtual boost::shared_ptr<ComputePipeline> CreateComputePipeline(const std::string& pipelineName, const boost::shared_ptr<Shader>& pShader) override final;

			virtual boost::shared_ptr<GeometryStore> CreateGeometryStore(const boost::unordered::unordered_map<UI32, boost::container::vector<ShaderAttribute>>& vertexAttributes, UI64 indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::AUTOMATIC) override final;

			virtual void SubmitGraphicsCommandBuffers(const boost::container::vector<boost::shared_ptr<CommandBuffer>>& pCommandBuffers) override final;
			virtual void SubmitComputeCommandBuffers(const boost::container::vector<boost::shared_ptr<CommandBuffer>>& pCommandBuffers) override final;
			virtual void WaitIdle() override final;
			virtual void WaitForQueue() override final;

			virtual void Terminate() override final;

		public:
			VkDevice GetLogicalDevice() const noexcept { return vLogicalDevice; }
			VkPhysicalDevice GetPhysicalDevice() const noexcept { return vPhysicalDevice; }

			VulkanQueue& GetQueue() { return vQueue; }
			const VulkanQueue GetQueue() const { return vQueue; }
			VkSampleCountFlags GetSampleCount() const { return vSampleCount; }

			VkResult CreateImageMemory(const boost::container::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const;
			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, const UI32 mipLevels = 1) const;
			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, const UI32 mipLevels = 1) const;

		private:
			void InitializePhysicalDevice();

			void InitializeLogicalDevice();
			void TerminateLogicalDevice();

		private:
			VulkanQueue vQueue = {};

			boost::container::vector<const char*> mDeviceExtensions;

			VkDevice vLogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;

			VkSampleCountFlags vSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;

			VkFence vSubmitFence = VK_NULL_HANDLE;
		};
	}
}