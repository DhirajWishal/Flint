// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Rasterizer.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanDescriptorSetManager.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRasterizer;

		/**
		 * Vulkan graphics pipeline class.
		 * This pipeline is used to perform raster graphics.
		 */
		class VulkanGraphicsPipeline final : public VulkanPipeline, public VulkanDescriptorSetManager
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param engine The engine to which the pipeline is bound to.
			 * @param rasterizer The rasterizer to which the pipeline is bound to.
			 * @param specification The pipeline specification.
			 * @param inputBindings The input bindings of the vertex shader.
			 * @param inputAttributes The input attributes of the vertex shader.
			 */
			explicit VulkanGraphicsPipeline(VulkanEngine& engine, VulkanRasterizer& rasterizer, const RasterizingPipelineSpecification& specification, std::vector<VkVertexInputBindingDescription>&& inputBindings, std::vector<VkVertexInputAttributeDescription>&& inputAttributes);

			/**
			 * Destructor.
			 */
			~VulkanGraphicsPipeline() override;

			/**
			 * Recreate the pipeline.
			 */
			void recreate();

		private:
			/**
			 * Resolve the shader information.
			 *
			 * @param code The shader code.
			 * @param stageFlag The shader stage flag.
			 * @param layoutBinding The descriptor set layout bindings.
			 * @param pushConstants The push constants.
			 */
			void resolveShader(const Shader& code, VkShaderStageFlagBits stageFlag, std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, std::vector<VkPushConstantRange>& pushConstants);

			/**
			 * Create the pipeline layout.
			 *
			 * @param pushConstants The push constants to bind.
			 */
			void createPipelineLayout(std::vector<VkPushConstantRange>&& pushConstants);

			/**
			 * Setup the default structures.
			 *
			 * @param specification The pipeline specification.
			 */
			void setupDefaults(const RasterizingPipelineSpecification& specification);

			/**
			 * Create the pipeline.
			 */
			void createPipeline();

			/**
			 * Destroy the created shaders.
			 */
			void destroyShaders();

		private:
			VkPipelineVertexInputStateCreateInfo m_VertexInputStateCreateInfo = {};
			VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyStateCreateInfo = {};
			VkPipelineTessellationStateCreateInfo m_TessellationStateCreateInfo = {};
			VkPipelineColorBlendStateCreateInfo m_ColorBlendStateCreateInfo = {};
			VkPipelineRasterizationStateCreateInfo m_RasterizationStateCreateInfo = {};
			VkPipelineMultisampleStateCreateInfo m_MultisampleStateCreateInfo = {};
			VkPipelineDepthStencilStateCreateInfo m_DepthStencilStateCreateInfo = {};
			VkPipelineDynamicStateCreateInfo m_DynamicStateCreateInfo = {};

			std::vector<VkPipelineColorBlendAttachmentState> m_CBASS = {};
			std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfo = {};
			std::vector<VkVertexInputBindingDescription> m_VertexBindings = {};
			std::vector<VkVertexInputAttributeDescription> m_VertexAttributes = {};
			std::vector<VkDynamicState> m_DynamicStates = {};

			VulkanRasterizer& m_Rasterizer;
		};
	}
}