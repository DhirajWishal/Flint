// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/RasterizingPipeline.hpp"
#include "VulkanDevice.hpp"

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
		class VulkanRasterizingPipeline final : public RasterizingPipeline
		{
			/**
			 * Pipeline structure.
			 * This contains information regarding a single pipeline.
			 */
			struct Pipeline final
			{
				VkPipelineVertexInputStateCreateInfo m_InputState = {};
				VkPipeline m_Pipeline = VK_NULL_HANDLE;
				VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
			};

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
			 * Attach a static model to the pipeline to render.
			 *
			 * @param pModel The model to attach.
			 * @return The draw entry for the model.
			 */
			[[nodiscard]] std::shared_ptr<DrawEntry> attach(const std::shared_ptr<StaticModel>& pModel) override;

			/**
			 * Load the pipeline cache from the handler if possible.
			 *
			 * @param identifier The pipeline identifier.
			 * @return The loaded pipeline cache.
			 */
			[[nodiscard]] VkPipelineCache loadCache(uint64_t identifier) const;

			/**
			 * Save the pipeline cache from the handler if possible.
			 *
			 * @param identifier The pipeline identifier.
			 * @param cache The pipeline cache to save.
			 */
			void saveCache(uint64_t identifier, VkPipelineCache cache) const;

		private:
			/**
			 * Setup the default structures.
			 *
			 * @param specification The pipeline specification.
			 */
			void setupDefaults(const RasterizingPipelineSpecification& specification);

			/**
			 * Create the pipeline variation as required by a model.
			 *
			 * @param inputState The pipeline input state.
			 * @param cache The pipeline cache.
			 * @return The created pipeline.
			 */
			[[nodiscard]] VkPipeline createVariation(const VkPipelineVertexInputStateCreateInfo& inputState, VkPipelineCache cache);

		private:
			std::unordered_map<uint64_t, Pipeline> m_Pipelines;

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