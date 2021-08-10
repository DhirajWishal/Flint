// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"
#include "VulkanInstance.hpp"
#include "VulkanQueue.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice final : public Device, public std::enable_shared_from_this<VulkanDevice>
		{
		public:
			VulkanDevice(const std::shared_ptr<Instance>& pInstance, DeviceFlags flags);

			virtual bool IsDisplayCompatible(const std::shared_ptr<Display>& pDisplay) override final;

			virtual std::shared_ptr<CommandBufferList> CreatePrimaryCommandBufferList(UI32 bufferCount) override final;
			virtual std::shared_ptr<CommandBufferList> CreateSecondaryCommandBufferList(UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pParent) override final;
			virtual void DestroyCommandBufferList(const std::shared_ptr<CommandBufferList>& pCommandBufferList) override final;

			virtual std::shared_ptr<ScreenBoundRenderTarget> CreateScreenBoundRenderTarget(const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, UI32 threadCount = 0) override final;
			virtual std::shared_ptr<OffScreenRenderTarget> CreateOffScreenRenderTarget(const FBox2D& extent, const UI32 bufferCount, OffScreenRenderTargetAttachment attachments = OffScreenRenderTargetAttachment::COLOR_BUFFER | OffScreenRenderTargetAttachment::DEPTH_BUFFER, UI32 threadCount = 0) override final;
			virtual void DestroyRenderTarget(const std::shared_ptr<RenderTarget>& pRenderTarget) override final;

			virtual std::shared_ptr<Buffer> CreateBuffer(BufferType type, UI64 size, BufferMemoryProfile profile = BufferMemoryProfile::AUTOMATIC) override final;
			virtual void DestroyBuffer(const std::shared_ptr<Buffer>& pBuffer) override final;

			virtual std::shared_ptr<Image> CreateImage(const ImageType type, ImageUsage usage, const FBox3D& extent, PixelFormat format, UI8 layers, UI32 mipLevels, const void* pImageData) override final;
			virtual void DestroyImage(const std::shared_ptr<Image>& pImage) override final;

			virtual std::shared_ptr<ImageSampler> CreateImageSampler(const ImageSamplerSpecification& specification) override final;
			virtual void DestroyImageSampler(const std::shared_ptr<ImageSampler>& pSampler) override final;

			virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::filesystem::path& path) override final;
			virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::vector<UI32>& code) override final;
			virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::string& code) override final;
			virtual void DestroyShader(const std::shared_ptr<Shader>& pShader) override final;

			virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(
				const std::string& pipelineName,
				const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
				const std::shared_ptr<Shader>& pVertexShader,
				const std::shared_ptr<Shader>& pTessellationControlShader,
				const std::shared_ptr<Shader>& pTessellationEvaluationShader,
				const std::shared_ptr<Shader>& pGeometryShader,
				const std::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification) override final;
			virtual void DestroyPipeline(const std::shared_ptr<Pipeline>& pPipeline) override final;

			virtual std::shared_ptr<GeometryStore> CreateGeometryStore(const std::unordered_map<UI32, std::vector<ShaderAttribute>>& vertexAttributes, UI64 indexSize, BufferMemoryProfile profile = BufferMemoryProfile::AUTOMATIC) override final;
			virtual void DestroyGeometryStore(const std::shared_ptr<GeometryStore>& pGeometryStore) override final;

			virtual void WaitIdle() override final;
			virtual void WaitForQueue() override final;

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