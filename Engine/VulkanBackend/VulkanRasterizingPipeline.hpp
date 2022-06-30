// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/RasterizingPipeline.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanDescriptorSetManager.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRasterizer;
		class VulkanRasterizingProgram;

		/**
		 * Vulkan rasterizing pipeline class.
		 * This pipeline is used to perform raster graphics.
		 */
		class VulkanRasterizingPipeline final : public RasterizingPipeline, public VulkanPipeline, public VulkanDescriptorSetManager
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device to which the pipeline is bound to.
			 * @param pRasterizer The rasterizer to which the pipeline is bound to.
			 * @param pProgram The rasterizing program used in the pipeline.
			 * @param specification The pipeline specification.
			 * @param pCacheHandler The pipeline cache handler. Default is nullptr.
			 */
			explicit VulkanRasterizingPipeline(const std::shared_ptr<VulkanDevice>& pDevice, const std::shared_ptr<VulkanRasterizer>& pRasterizer, const std::shared_ptr<VulkanRasterizingProgram>& pProgram, const RasterizingPipelineSpecification& specification, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler = nullptr);

			/**
			 * Destructor.
			 */
			~VulkanRasterizingPipeline() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

			/**
			 * Recreate the pipeline.
			 */
			void recreate();

			/**
			 * Load the pipeline cache from the handler if possible.
			 */
			void loadCache() override;

			/**
			 * Save the pipeline cache from the handler if possible.
			 */
			void saveCache() override;

		private:
			/**
			 * Resolve the shader information.
			 *
			 * @param code The shader code.
			 * @param stageFlag The shader stage flag.
			 * @param layoutBinding The descriptor set layout bindings.
			 * @param pushConstants The push constants.
			 */
			 //void resolveShader(const Shader& code, VkShaderStageFlagBits stageFlag, std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, std::vector<VkPushConstantRange>& pushConstants);

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
		};
	}
}