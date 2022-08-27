// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Backend/RasterizingPipeline.hpp"
#include "VulkanDevice.hpp"
#include "VulkanDescriptorSetManager.hpp"

#include <thread>
#include <condition_variable>

namespace Flint
{
	namespace Backend
	{
		class VulkanRasterizer;
		class VulkanRasterizingProgram;
		class VulkanCommandBuffers;

		/**
		 * Vulkan rasterizing pipeline class.
		 * This pipeline is used to perform raster graphics.
		 */
		class VulkanRasterizingPipeline final : public std::enable_shared_from_this<VulkanRasterizingPipeline>, public RasterizingPipeline
		{
			/**
			 * Pipeline structure.
			 * This contains information regarding a single pipeline.
			 */
			struct Pipeline final
			{
				std::vector<VkVertexInputBindingDescription> m_InputBindings;
				std::vector<VkVertexInputAttributeDescription> m_InputAttributes;

				VkPipeline m_Pipeline = VK_NULL_HANDLE;
				VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
			};

			/**
			 * Draw call function type.
			 * This is used to store draw calls for each model.
			 *
			 * @param The command buffers used to record the commands to.
			 * @param The current frame index.
			 */
			using DrawCall = std::function<void(const VulkanCommandBuffers&, uint32_t)>;

			/**
			 * Worker payload structure.
			 */
			struct WorkerPayload final
			{
				VkCommandBufferInheritanceInfo m_InheritanceInfo = {};
				uint32_t m_FrameIndex = 0;
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
			 * @param binder The mesh binder used to bind resources to meshes.
			 * @return The draw entry for the model.
			 */
			[[nodiscard]] std::shared_ptr<DrawEntry> attach(const std::shared_ptr<StaticModel>& pModel, ResourceBinder&& binder) override;

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

			/**
			 * Notify the render target to update.
			 */
			void notifyRenderTarget();

			/**
			 * Get the draw entries.
			 *
			 * @return The draw entries.
			 */
			[[nodiscard]] const std::vector<std::shared_ptr<DrawEntry>>& getDrawEntries() const { return m_pDrawEntries; }

			/**
			 * Get the descriptor set manager.
			 *
			 * @return The descriptor set manager.
			 */
			[[nodiscard]] const VulkanDescriptorSetManager& getDescriptorSetManager() const { return m_DescriptorSetManager; }

			/**
			 * Draw the bind resources.
			 *
			 * @param inheritanceInfo The command buffer inheritance info.
			 * @param frameIndex The current frame index.
			 * @param shouldWait Whether or not we need to wait till the thread finishes it's execution. Default is false.
			 */
			void draw(VkCommandBufferInheritanceInfo inheritanceInfo, uint32_t frameIndex, bool shouldWait = false);

			/**
			 * Issue all the draw calls.
			 *
			 * @param commandBuffers The command buffers to record the commands.
			 * @param frameIndex The current frame index.
			 */
			void issueDrawCalls(const VulkanCommandBuffers& commandBuffers, uint32_t frameIndex) const;

			/**
			 * Get the pipeline handle.
			 *
			 * @param identifier The pipeline identifier.
			 * @return The Vulkan pipeline handle.
			 */
			[[nodiscard]] VkPipeline getPipelineHandle(uint64_t identifier) const { return m_Pipelines.at(identifier).m_Pipeline; }

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
			[[nodiscard]] VkPipeline createVariation(VkPipelineVertexInputStateCreateInfo&& inputState, VkPipelineCache cache);

			/**
			 * Worker function.
			 * This function is used to bind resources to secondary command buffers.
			 */
			void worker();

			/**
			 * Terminate the worker.
			 */
			void terminateWorker();

		private:
			std::jthread m_WorkerThread;
			std::condition_variable m_ConditionVariable;
			std::mutex m_WorkerMutex;
			std::atomic<bool> m_ShouldRun = true;
			std::atomic<bool> m_Updated = false;

			std::unordered_map<uint64_t, Pipeline> m_Pipelines;
			VulkanDescriptorSetManager m_DescriptorSetManager;

			VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyStateCreateInfo = {};
			VkPipelineTessellationStateCreateInfo m_TessellationStateCreateInfo = {};
			VkPipelineColorBlendStateCreateInfo m_ColorBlendStateCreateInfo = {};
			VkPipelineRasterizationStateCreateInfo m_RasterizationStateCreateInfo = {};
			VkPipelineMultisampleStateCreateInfo m_MultisampleStateCreateInfo = {};
			VkPipelineDepthStencilStateCreateInfo m_DepthStencilStateCreateInfo = {};
			VkPipelineDynamicStateCreateInfo m_DynamicStateCreateInfo = {};

			WorkerPayload m_WorkerPayload = {};
			std::shared_ptr<VulkanCommandBuffers> m_pSecondaryCommandBuffers = nullptr;

			std::vector<VkPipelineColorBlendAttachmentState> m_CBASS = {};
			std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfo = {};
			std::vector<VkDynamicState> m_DynamicStates = {};

			std::vector<std::shared_ptr<DrawEntry>> m_pDrawEntries;
			std::vector<DrawCall> m_DrawCalls;
		};
	}
}