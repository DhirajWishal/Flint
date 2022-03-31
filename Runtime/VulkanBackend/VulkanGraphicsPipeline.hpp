// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanGraphicsPipeline final : public GraphicsPipeline<VulkanDevice, RenderTarget<VulkanDevice, VulkanImage>, VulkanResourcePackage, VulkanShader, VulkanScreenBoundRenderTarget, VulkanOffScreenRenderTarget>
		{
		public:
			explicit VulkanGraphicsPipeline(VulkanDevice* pDevice, const std::string& pipelineName, const VulkanScreenBoundRenderTarget* pScreenBoundRenderTarget, std::unique_ptr<VulkanShader>&& pVertexShader, std::unique_ptr<VulkanShader>&& pTessellationControlShader, std::unique_ptr<VulkanShader>&& pTessellationEvaluationShader, std::unique_ptr<VulkanShader>&& pGeometryShader, std::unique_ptr<VulkanShader>&& pFragmentShader, const GraphicsPipelineSpecification& specification);
			explicit VulkanGraphicsPipeline(VulkanDevice* pDevice, const std::string& pipelineName, const VulkanOffScreenRenderTarget* pOffScreenRenderTarget, std::unique_ptr<VulkanShader>&& pVertexShader, std::unique_ptr<VulkanShader>&& pTessellationControlShader, std::unique_ptr<VulkanShader>&& pTessellationEvaluationShader, std::unique_ptr<VulkanShader>&& pGeometryShader, std::unique_ptr<VulkanShader>&& pFragmentShader, const GraphicsPipelineSpecification& specification);
			~VulkanGraphicsPipeline() { if (!bIsTerminated) Terminate(); }

			virtual void ReloadShaders() override;
			virtual void Recreate(VulkanScreenBoundRenderTarget* pScreenBoundRenderTarget) override;
			virtual void CreateResourcePackagers() override;
			virtual void Terminate() override;

			const VkPipelineLayout GetPipelineLayout() const { return m_vPipelineLayout; }
			const VkPipeline GetPipeline() const { return m_vPipeline; }
			const std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts() const { return m_vDescriptorSetLayouts; }

		private:
			void SetupDefaults();
			void CreatePipelineCache();
			void CreatePipelineLayout();
			void CreatePipeline();

		private:
			VkPipelineVertexInputStateCreateInfo m_vVertexInputStateCreateInfo = {};
			VkPipelineInputAssemblyStateCreateInfo m_vInputAssemblyStateCreateInfo = {};
			VkPipelineTessellationStateCreateInfo m_vTessellationStateCreateInfo = {};
			VkPipelineColorBlendStateCreateInfo m_vColorBlendStateCreateInfo = {};
			VkPipelineRasterizationStateCreateInfo m_vRasterizationStateCreateInfo = {};
			VkPipelineMultisampleStateCreateInfo m_vMultisampleStateCreateInfo = {};
			VkPipelineDepthStencilStateCreateInfo m_vDepthStencilStateCreateInfo = {};
			VkPipelineDynamicStateCreateInfo m_vDynamicStateCreateInfo = {};

			std::vector<VkPipelineColorBlendAttachmentState> m_vCBASS = {};
			std::vector<VkPipelineShaderStageCreateInfo> m_vShaderStageCreateInfo = {};
			std::vector<VkVertexInputAttributeDescription> m_vVertexAttributes = {};
			std::vector<VkVertexInputBindingDescription> m_vVertexBindings = {};
			std::vector<VkDynamicState> m_vDynamicStates = {};

			std::vector<VkDescriptorSetLayout> m_vDescriptorSetLayouts = {};

			VkPipelineLayout m_vPipelineLayout = VK_NULL_HANDLE;
			VkPipeline m_vPipeline = VK_NULL_HANDLE;
			VkPipelineCache m_vPipelineCache = VK_NULL_HANDLE;

			VkRenderPass m_vRenderPass = VK_NULL_HANDLE;
		};
	}
}