// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\FPipelineResource.h"
#include "VulkanPipeline.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanPipelineResource : public FPipelineResource {
			friend VulkanPipeline;

		public:
			using DeviceType = VulkanDevice;

		public:
			VulkanPipelineResource(std::shared_ptr<FGraphicsPipeline> pPipeline);

			virtual void Terminate() override final;
			virtual void RegisterUniformBuffers(const UniformBufferContainer& uniformBuffers) override final;
			virtual void RegisterUniformImages(const UniformImageContainer& unformImages) override final;

			VkPipelineBindPoint GetBindPoint() const { return pvPipeline->GetBindPoint(); }
			VkPipelineLayout GetPipelineLayout() const { return pvPipeline->vPipelineLayout; }

			VkDescriptorPool vPool = VK_NULL_HANDLE;
			VkDescriptorSet vDescriptorSet = VK_NULL_HANDLE;

		private:
			void CreateDescriptorPool(const std::vector<VkDescriptorPoolSize>& vSizes);

			std::unordered_map<SamplerSpecification, VkSampler> mSamplerMap;

			VulkanPipeline* pvPipeline = nullptr;
		};
	}
}