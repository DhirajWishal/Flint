// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanGraphicsPipeline final : public GraphicsPipeline, public boost::shared_ptr<VulkanGraphicsPipeline>
		{
		public:
			VulkanGraphicsPipeline(
				const boost::shared_ptr<Device>& pDevice,
				const std::string& pipelineName,
				const boost::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
				const boost::shared_ptr<Shader>& pVertexShader,
				const boost::shared_ptr<Shader>& pTessellationControlShader,
				const boost::shared_ptr<Shader>& pTessellationEvaluationShader,
				const boost::shared_ptr<Shader>& pGeometryShader,
				const boost::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification);

			VulkanGraphicsPipeline(
				const boost::shared_ptr<Device>& pDevice,
				const std::string& pipelineName,
				const boost::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget,
				const boost::shared_ptr<Shader>& pVertexShader,
				const boost::shared_ptr<Shader>& pTessellationControlShader,
				const boost::shared_ptr<Shader>& pTessellationEvaluationShader,
				const boost::shared_ptr<Shader>& pGeometryShader,
				const boost::shared_ptr<Shader>& pFragmentShader,
				const GraphicsPipelineSpecification& specification);
			~VulkanGraphicsPipeline() { if (!bIsTerminated) Terminate(); }

			virtual void ReloadShaders() override final;
			virtual void Recreate(const boost::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget) override final;
			virtual void Terminate() override final;

			virtual void PrepareResources() override final;

			const VkPipelineLayout GetPipelineLayout() const { return vPipelineLayout; }
			const VkPipeline GetPipeline() const { return vPipeline; }
			const VkDescriptorSet GetDescriptorSet(const boost::shared_ptr<ResourceMap>& pResourceMap) const;
			const VkDescriptorSet* GetDescriptorSetAddress(const boost::shared_ptr<ResourceMap>& pResourceMap) const;

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

			boost::container::vector<VkPipelineColorBlendAttachmentState> vCBASS = {};
			boost::container::vector<VkPipelineShaderStageCreateInfo> vShaderStageCreateInfo = {};
			boost::container::vector<VkVertexInputAttributeDescription> vVertexAttributes = {};
			boost::container::vector<VkVertexInputBindingDescription> vVertexBindings = {};
			boost::container::vector<VkDynamicState> vDynamicStates = {};

			boost::unordered::unordered_map<boost::shared_ptr<ResourceMap>, VkDescriptorSet> vDescriptorSetMap;

			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineCache vPipelineCache = VK_NULL_HANDLE;

			VkRenderPass vRenderPass = VK_NULL_HANDLE;

			VkDescriptorSetLayout vDescriptorSetLayout = VK_NULL_HANDLE;
			VkDescriptorPool vDescriptorSetPool = VK_NULL_HANDLE;
		};
	}
}