// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanEngine.hpp"

namespace Flint
{
	class Mesh;

	namespace VulkanBackend
	{
		class VulkanWindow;
		class VulkanRasterizer;
		class VulkanGeometryStore;
		class VulkanGraphicsPipeline;

		/**
		 * Vulkan command buffers class.
		 * This contains the command buffers needed for certain actions.
		 */
		class VulkanCommandBuffers final
		{
			/**
			 * Fence structure.
			 * This contains the actual fence and a boolean stating if it's submitted and is free or not.
			 *
			 * If a fence is free, we don't need to wait.
			 */
			struct Fence
			{
				VkFence m_Fence = VK_NULL_HANDLE;
				bool m_IsFree = true;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param engine The engine to which the command buffer is bound to.
			 * @param bufferCount The number of command buffers.
			 * @param level The command buffer level. Default is primary.
			 */
			explicit VulkanCommandBuffers(VulkanEngine& engine, uint32_t bufferCount, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

			/**
			 * Explicit constructor.
			 * This will create a utility command buffer.
			 *
			 * @param engine The engine to which the command buffer is bound to.
			 * @param level The command buffer level. Default is primary.
			 */
			explicit VulkanCommandBuffers(VulkanEngine& engine, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

			/**
			 * Destructor.
			 */
			~VulkanCommandBuffers();

			/**
			 * Begin the command buffer.
			 *
			 * @param pInheritanceInfo The inheritance info to pass down to the command buffer. Default is nullptr.
			 */
			void begin(VkCommandBufferInheritanceInfo* pInheritanceInfo = nullptr);

			/**
			 * Bind a window to the command buffer.
			 *
			 * @param window The window to bind.
			 * @param clearColors The clear colors to bind.
			 */
			void bindWindow(const VulkanWindow& window, const std::vector<VkClearValue>& clearColors) const noexcept;

			/**
			 * Unbind the currently bound window.
			 */
			void unbindWindow() const;

			/**
			 * Bind a rasterizer to the command buffer.
			 *
			 * @param rasterizer The rasterizer to bind.
			 * @param clearColors The clear colors to bind.
			 */
			void bindRenderTarget(const VulkanRasterizer& rasterizer, const std::vector<VkClearValue>& clearColors) const noexcept;

			/**
			 * Unbind the bound render target.
			 */
			void unbindRenderTarget() const;

			/**
			 * Change the image layout of an image.
			 *
			 * @param image The image to change the layout of.
			 * @param currentLayout The current layout of the image.
			 * @param newLayout The new layout to change to.
			 * @param aspectFlags The image aspect flags.
			 */
			void changeImageLayout(VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags) const;

			/**
			 * Copy content from one buffer to another.
			 *
			 * @param srcBuffer The source buffer.
			 * @param size The size to copy.
			 * @param srcOffset The offset of the source buffer to copy.
			 * @param dstBuffer The destination buffer.
			 * @param dstOffset The destination offset.
			 */
			void copyBuffer(VkBuffer srcBuffer, uint64_t size, uint64_t srcOffset, VkBuffer dstBuffer, uint64_t dstOffset) const noexcept;

			/**
			 * Bind an index buffer from a geometry store to the current command buffer.
			 *
			 * @param geometryStore The geometry store to bind.
			 */
			void bindIndexBuffer(const VulkanGeometryStore& geometryStore) const noexcept;

			/**
			 * Bind a graphics pipeline to the command buffer.
			 *
			 * @param pipeline The pipeline to bind.
			 */
			void bindGraphicsPipeline(const VulkanGraphicsPipeline& pipeline) const noexcept;

			/**
			 * Bind a graphics descriptor to the command buffer.
			 *
			 * @param pipeline The pipeline to which the descriptor is bound to.
			 * @param descriptorSet The descriptor set to bind.
			 */
			void bindDescriptor(const VulkanGraphicsPipeline& pipeline, VkDescriptorSet descriptorSet) const noexcept;

			/**
			 * Draw a mesh.
			 *
			 * @param geometryStore The geometry store to bind.
			 * @param mesh The mesh to draw.
			 */
			void drawMesh(const VulkanGeometryStore& geometryStore, const Mesh& mesh) const noexcept;

			/**
			 * End recording.
			 */
			void end();

			/**
			 * Submit the command buffer to the GPU to be executed.
			 *
			 * @param renderFinishedSemaphore The render finished semaphore to be signaled.
			 * @param inFlightSemaphore The in flight semaphore.
			 * @param waitStageMask The wait stage mask to wait till completion. Default is color attachment output.
			 */
			void submit(VkSemaphore renderFinishedSemaphore, VkSemaphore inFlightSemaphore, VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

			/**
			 * Submit the command buffer to the GPU to be executed.
			 * This does not do any signaling operations.
			 *
			 * @param waitStageMask The wait stage mask to wait till completion. Default is color attachment output.
			 */
			void submitGraphics(VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

			/**
			 * Submit transfer commands.
			 */
			void submitTransfer();

			/**
			 * Submit compute commands.
			 */
			void submitCompute();

			/**
			 * Wait till a command buffer finishes execution.
			 */
			void finishExecution();

			/**
			 * Select the next command buffer as the current buffer.
			 */
			void next();

			/**
			 * Reset the command buffer index to 0.
			 */
			void resetIndex();

			/**
			 * Get the current command buffer.
			 *
			 * @return The command buffer.
			 */
			[[nodiscard]] VkCommandBuffer getCurrentBuffer() const { return m_CurrentCommandBuffer; }

			/**
			 * Get the current command pool.
			 *
			 * @return The command pool.
			 */
			[[nodiscard]] VkCommandPool getCommandPool() const { return m_CommandPool; }

			/**
			 * Check if the current command buffer is in the recording state.
			 *
			 * @return If the current command buffer is in the recording state or not.
			 */
			[[nodiscard]] bool isRecording() const { return m_IsRecording; }

		private:
			/**
			 * Create the fences.
			 */
			void createFences();

			/**
			 * Destroy the fences.
			 */
			void destroyFences();

		private:
			std::vector<VkCommandBuffer> m_CommandBuffers;
			std::vector<Fence> m_CommandFences;

			VulkanEngine& m_Engine;
			VkCommandPool m_CommandPool = VK_NULL_HANDLE;
			VkCommandBuffer m_CurrentCommandBuffer = VK_NULL_HANDLE;

			uint32_t m_CurrentIndex = 0;

			bool m_IsRecording = false;
		};
	}
}