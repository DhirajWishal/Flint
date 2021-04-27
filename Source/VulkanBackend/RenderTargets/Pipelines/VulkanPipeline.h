// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>

#include "Core\Backend\FPipeline.h"
#include "VulkanBackend\VulkanBuffer.h"
#include "VulkanBackend\VulkanImage.h"
#include "VulkanBackend\RenderTargets\VulkanRenderTarget.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanPipeline;

		class VulkanPipelineResource : public FPipelineResource {
			friend VulkanPipeline;

		public:
			using DeviceType = VulkanDevice;

		public:
			VulkanPipelineResource(std::shared_ptr<FPipeline> pPipeline);
			~VulkanPipelineResource();

			virtual void RegisterUniformBuffers(const UniformBufferContainer& uniformBuffers) override final;
			virtual void RegisterUniformImages(const UniformImageContainer& unformImages) override final;

			VkDescriptorPool vPool = VK_NULL_HANDLE;
			VkDescriptorSet vDescriptorSet = VK_NULL_HANDLE;

		private:
			void CreateDescriptorPool(const std::vector<VkDescriptorPoolSize>& vSizes);

			std::unordered_map<SamplerSpecification, VkSampler> mSamplerMap;
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