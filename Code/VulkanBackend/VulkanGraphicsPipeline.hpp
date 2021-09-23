// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanGraphicsPipeline final : public GraphicsPipeline, public std::enable_shared_from_this<VulkanGraphicsPipeline>
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
			~VulkanGraphicsPipeline() { if (!bIsTerminated) Terminate(); }

			virtual void ReloadShaders() override;
			virtual void Recreate(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget) override;
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