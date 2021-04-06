// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>

#include "Core\Backend\Pipeline.h"
#include "VulkanBackend\VulkanBuffer.h"
#include "VulkanBackend\VulkanImage.h"
#include "VulkanBackend\RenderTargets\VulkanRenderTarget.h"
#include "VulkanBackend\VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanPipeline;

		class VulkanPipelineResource : public Backend::PipelineResource<VulkanPipelineResource, VulkanPipeline, VulkanDevice, VulkanBuffer, VulkanImage> {
			friend VulkanPipeline;

		public:
			using DeviceType = VulkanDevice;

		public:
			VulkanPipelineResource() {}
			~VulkanPipelineResource() {}

			virtual void Initialize(VulkanPipeline* pPipeline) override final;
			virtual void Terminate() override final;

			virtual void RegisterUniformBuffers(const Backend::UniformBufferContainer<VulkanBuffer>& uniformBuffers) override final;
			virtual void RegisterUniformImages(const Backend::UniformImageContainer<VulkanImage>& unformImages) override final;

			VkDescriptorPool vPool = VK_NULL_HANDLE;
			VkDescriptorSet vDescriptorSet = VK_NULL_HANDLE;

		private:
			void CreateDescriptorPool(const std::vector<VkDescriptorPoolSize>& vSizes);

			std::unordered_map<Backend::SamplerSpecification, VkSampler> mSamplerMap;
		};

		class VulkanPipeline {
		public:
			VulkanPipeline() {}
			virtual ~VulkanPipeline() {}

			virtual VkPipelineBindPoint GetBindPoint() const = 0;
			virtual VulkanRenderTarget* GetRenderTarget() const = 0;
			virtual std::unordered_map<String, UniformLayout> GetUniformLayouts() const = 0;
			virtual void RecreatePipeline() = 0;

			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout vSetLayout = VK_NULL_HANDLE;

			std::vector<VkDescriptorPoolSize> vPoolSizes;
		};
	}
}