// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ComputePipeline.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanComputePipeline final : public ComputePipeline, public std::enable_shared_from_this<VulkanComputePipeline>
		{
		public:
			VulkanComputePipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<Shader>& pComputeShader);
			~VulkanComputePipeline() { if (!bIsTerminated) Terminate(); }

			virtual void ReloadShaders() override;
			void CreateResourcePackagers() override;

			virtual void Terminate() override;

			const VkPipelineLayout GetPipelineLayout() const { return vPipelineLayout; }
			const VkPipeline GetPipeline() const { return vPipeline; }

		private:
			void CreatePipelineCache();
			void CreatePipelineLayout();
			void CreatePipeline();

		private:
			std::shared_ptr<ComputePipeline> pThisPipeline = nullptr;
			std::vector<VkDescriptorSetLayout> vDescriptorSetLayouts = {};

			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineCache vPipelineCache = VK_NULL_HANDLE;
		};
	}
}