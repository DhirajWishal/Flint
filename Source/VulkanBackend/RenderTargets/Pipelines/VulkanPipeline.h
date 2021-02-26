// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Pipeline.h"
#include "VulkanBackend/RenderTargets/VulkanRenderTarget.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanPipeline;

		class VulkanPipelineResource : public Backend::PipelineResource {
			friend VulkanPipeline;

		public:
			VulkanPipelineResource() {}
			~VulkanPipelineResource() {}

			virtual void Initialize(Backend::Pipeline* pPipeline) override final;
			virtual void Terminate() override final;

			virtual void RegisterUniformBuffers(const Backend::UniformBufferContainer& uniformBuffers) override final;
		
			virtual void Bind(const Backend::CommandBuffer& commandBuffer) override final;

		private:
			void CreateDescriptorPool(const std::vector<VkDescriptorPoolSize>& vSizes);

		private:
			VkDescriptorPool vPool = VK_NULL_HANDLE;
			VkDescriptorSet vDescriptorSet = VK_NULL_HANDLE;
		};

		class VulkanPipeline {
			friend VulkanPipelineResource;

		public:
			VulkanPipeline() {}
			virtual ~VulkanPipeline() {}

			virtual VkPipelineBindPoint GetBindPoint() = 0;

		protected:
			std::vector<VkDescriptorPoolSize> vPoolSizes;

			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout vSetLayout = VK_NULL_HANDLE;
		};
	}
}