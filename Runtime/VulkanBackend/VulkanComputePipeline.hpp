// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ComputePipeline.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanComputePipeline final : public ComputePipeline<VulkanDevice, RenderTarget<VulkanDevice, VulkanImage>, VulkanResourcePackage, VulkanShader>
		{
		public:
			explicit VulkanComputePipeline(VulkanDevice* pDevice, const std::string& pipelineName, std::unique_ptr<VulkanShader>&& pComputeShader);
			~VulkanComputePipeline() { if (!bIsTerminated) Terminate(); }

			virtual void ReloadShaders() override;
			void CreateResourcePackagers() override;

			virtual void Terminate() override;

			const VkPipelineLayout GetPipelineLayout() const { return m_vPipelineLayout; }
			const VkPipeline GetPipeline() const { return m_vPipeline; }

		private:
			void CreatePipelineCache();
			void CreatePipelineLayout();
			void CreatePipeline();

		private:
			std::vector<VkDescriptorSetLayout> m_vDescriptorSetLayouts = {};

			VkPipelineLayout m_vPipelineLayout = VK_NULL_HANDLE;
			VkPipeline m_vPipeline = VK_NULL_HANDLE;
			VkPipelineCache m_vPipelineCache = VK_NULL_HANDLE;
		};
	}
}