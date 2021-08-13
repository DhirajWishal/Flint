// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsPipeline.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanGraphicsPipeline final : public GraphicsPipeline, public std::shared_ptr<VulkanGraphicsPipeline>
		{
		public:
			VulkanGraphicsPipeline(
				const std::shared_ptr<Device>& pDevice,
				const std::string& pipelineName,
				const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
				const std::shared_ptr<Shader>& pVertexShader,
				const std::shared_ptr<Shader>& pTessellationControlShader,
				const std::shared_ptr<Shader>& pTessellationEvaluationShader,
				const std::shared_ptr<Shader>& pGeometryShader,
				const std::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification);

			VulkanGraphicsPipeline(
				const std::shared_ptr<Device>& pDevice,
				const std::string& pipelineName,
				const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget,
				const std::shared_ptr<Shader>& pVertexShader,
				const std::shared_ptr<Shader>& pTessellationControlShader,
				const std::shared_ptr<Shader>& pTessellationEvaluationShader,
				const std::shared_ptr<Shader>& pGeometryShader,
				const std::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification);

			virtual void Recreate(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget) override final;
			virtual void Terminate() override final;

			virtual void PrepareResourcesToDraw() override final;

			const VkPipelineLayout GetPipelineLayout() const { return vPipelineLayout; }
			const VkPipeline GetPipeline() const { return vPipeline; }
			const VkDescriptorSet GetDescriptorSet(const std::shared_ptr<ResourceMap>& pResourceMap) const;
			const VkDescriptorSet* GetDescriptorSetAddress(const std::shared_ptr<ResourceMap>& pResourceMap) const;

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

			std::unordered_map<std::shared_ptr<ResourceMap>, VkDescriptorSet> vDescriptorSetMap;

			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineCache vPipelineCache = VK_NULL_HANDLE;

			VkRenderPass vRenderPass = VK_NULL_HANDLE;

			VkDescriptorSetLayout vDescriptorSetLayout = VK_NULL_HANDLE;
			VkDescriptorPool vDescriptorSetPool = VK_NULL_HANDLE;
		};
	}
}