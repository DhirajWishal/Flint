// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanEngine.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanWindow;
		class VulkanRasterizer;

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
			void bindWindow(const VulkanWindow& window, const std::vector<VkClearValue>& clearColors) const;

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
			void bindRenderTarget(const VulkanRasterizer& rasterizer, const std::vector<VkClearValue>& clearColors) const;

			/**
			 * Unbind the bound render target.
			 */
			void unbindRenderTarget() const;

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
			void submit(VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

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

			/**
			 * Wait till a command buffer finishes execution.
			 */
			void finishExecution();

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