// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanGraphicsPipeline final : public GraphicsPipeline
		{
		public:
			VulkanGraphicsPipeline(Device* pDevice, const std::string& pipelineName, const ScreenBoundRenderTarget* pScreenBoundRenderTarget, std::unique_ptr<Shader>&& pVertexShader, std::unique_ptr<Shader>&& pTessellationControlShader, std::unique_ptr<Shader>&& pTessellationEvaluationShader, std::unique_ptr<Shader>&& pGeometryShader, std::unique_ptr<Shader>&& pFragmentShader, const GraphicsPipelineSpecification& specification);
			VulkanGraphicsPipeline(Device* pDevice, const std::string& pipelineName, const OffScreenRenderTarget* pOffScreenRenderTarget, std::unique_ptr<Shader>&& pVertexShader, std::unique_ptr<Shader>&& pTessellationControlShader, std::unique_ptr<Shader>&& pTessellationEvaluationShader, std::unique_ptr<Shader>&& pGeometryShader, std::unique_ptr<Shader>&& pFragmentShader, const GraphicsPipelineSpecification& specification);
			~VulkanGraphicsPipeline() { if (!bIsTerminated) Terminate(); }

			virtual void ReloadShaders() override;
			virtual void Recreate(ScreenBoundRenderTarget* pScreenBoundRenderTarget) override;
			virtual void CreateResourcePackagers() override;
			virtual void Terminate() override;

			const VkPipelineLayout GetPipelineLayout() const { return vPipelineLayout; }
			const VkPipeline GetPipeline() const { return vPipeline; }
			const std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts() const { return vDescriptorSetLayouts; }

		private:
			void SetupDefaults();
			void CreatePipelineCache();
			void CreatePipelineLayout();
			void CreatePipeline();

		private:
			VkPipelineVertexInputStateCreateInfo vVertexInputStateCreateInfo = {};
			VkPipelineInputAssemblyStateCreateInfo vInputAssemblyStateCreateInfo = {};
			VkPipelineTessellationStateCreateInfo vTessellationStateCreateInfo = {};
			VkPipelineColorBlendStateCreateInfo vColorBlendStateCreateInfo = {};
			VkPipelineRasterizationStateCreateInfo vRasterizationStateCreateInfo = {};
			VkPipelineMultisampleStateCreateInfo vMultisampleStateCreateInfo = {};
			VkPipelineDepthStencilStateCreateInfo vDepthStencilStateCreateInfo = {};
			VkPipelineDynamicStateCreateInfo vDynamicStateCreateInfo = {};

			std::vector<VkPipelineColorBlendAttachmentState> vCBASS = {};
			std::vector<VkPipelineShaderStageCreateInfo> vShaderStageCreateInfo = {};
			std::vector<VkVertexInputAttributeDescription> vVertexAttributes = {};
			std::vector<VkVertexInputBindingDescription> vVertexBindings = {};
			std::vector<VkDynamicState> vDynamicStates = {};

			std::vector<VkDescriptorSetLayout> vDescriptorSetLayouts = {};

			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineCache vPipelineCache = VK_NULL_HANDLE;

			VkRenderPass vRenderPass = VK_NULL_HANDLE;
		};
	}
}