// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/CommandBuffers.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanWindow;
		class VulkanRasterizer;
		class VulkanRasterizingPipeline;
		class VulkanVertexStorage;

		/**
		 * Vulkan command buffers class.
		 * This contains the command buffers needed for certain actions.
		 */
		class VulkanCommandBuffers final : public std::enable_shared_from_this<VulkanCommandBuffers>, public CommandBuffers
		{
			/**
			 * Fence structure.
			 * This contains the actual fence and a boolean stating if it's submitted and is free or not.
			 *
			 * If a fence is free, we don't need to wait.
			 */
			struct Fence final
			{
				VkFence m_Fence = VK_NULL_HANDLE;
				bool m_IsFree = true;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device to which the command buffer is bound to.
			 * @param bufferCount The number of command buffers.
			 * @param level The command buffer level. Default is primary.
			 */
			explicit VulkanCommandBuffers(const std::shared_ptr<VulkanDevice>& pDevice, uint32_t bufferCount, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

			/**
			 * Explicit constructor.
			 * This will create a utility command buffer.
			 *
			 * @param device The device to which the command buffer is bound to.
			 * @param level The command buffer level. Default is primary.
			 */
			explicit VulkanCommandBuffers(const std::shared_ptr<VulkanDevice>& pDevice, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

			/**
			 * Explicit constructor.
			 * Create the command buffer as a secondary command buffer.
			 *
			 * @param pParentCommandBuffers The parent command buffer pointer.
			 */
			explicit VulkanCommandBuffers(const std::shared_ptr<VulkanCommandBuffers>& pParentCommandBuffers);

			/**
			 * Destructor.
			 */
			~VulkanCommandBuffers() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

			/**
			 * Create child command buffers.
			 *
			 * @return The child command buffers.
			 */
			[[nodiscard]] std::shared_ptr<VulkanCommandBuffers> createChild();

			/**
			 * Begin the command buffer.
			 * Make sure that the command buffer has finished it's execution before calling this. It can be done by calling 'finishExecution()'
			 */
			void begin();

			/**
			 * Begin the command buffer.
			 * Make sure that the command buffer has finished it's execution before calling this. It can be done by calling 'finishExecution()'
			 *
			 * @param pInheritanceInfo The inheritance info to pass down to the command buffer.
			 */
			void begin(VkCommandBufferInheritanceInfo* pInheritanceInfo);

			/**
			 * Bind a window to the command buffer.
			 *
			 * @param window The window to bind.
			 * @param clearColor The clear color to bind.
			 */
			void bindWindow(const VulkanWindow& window, const VkClearValue& clearColor) const noexcept;

			/**
			 * Unbind the currently bound window.
			 */
			void unbindWindow() const;

			/**
			 * Bind a rasterizer to the command buffer.
			 *
			 * @param rasterizer The rasterizer to bind.
			 * @param clearColors The clear colors to bind.
			 * @param subpassContents The subpass contents. Default is VK_SUBPASS_CONTENTS_INLINE.
			 */
			void bindRenderTarget(const VulkanRasterizer& rasterizer, const std::vector<VkClearValue>& clearColors, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const noexcept;

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
			 * @param mipLevels The image mip levels. Default is 1.
			 * @param layers The image layers. Default is 1.
			 */
			void changeImageLayout(VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1, uint32_t layers = 1) const;

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
			 * Copy content from a buffer to an image.
			 *
			 * @param srcBuffer The source buffer.
			 * @param bufferHeight The height of the image in the buffer.
			 * @param bufferWidth The width of the image in the buffer.
			 * @param dstImage The destination image.
			 * @param layout The image layout.
			 * @param imageExtent The image extent.
			 * @param imageOffset The image offset.
			 * @param imageSubresource The image sub resource layers.
			 */
			void copyBufferToImage(VkBuffer srcBuffer, uint64_t bufferOffset, uint32_t bufferHeight, uint32_t bufferWidth, VkImage dstImage, VkImageLayout layout, VkExtent3D imageExtent, VkOffset3D imageOffset, VkImageSubresourceLayers imageSubresource) const noexcept;

			/**
			 * Copy content from an image to a buffer.
			 *
			 * @param srcImage The source image.
			 * @param layout The image layout.
			 * @param imageExtent The image extent.
			 * @param imageOffset The image offset.
			 * @param imageSubresource The image sub resource layers.
			 * @param dstBuffer The destination buffer.
			 * @param bufferHeight The height of the image in the buffer.
			 * @param bufferWidth The width of the image in the buffer.
			 */
			void copyImageToBuffer(VkImage srcImage, VkImageLayout layout, VkExtent3D imageExtent, VkOffset3D imageOffset, VkImageSubresourceLayers imageSubresource, VkBuffer dstBuffer, uint64_t bufferOffset, uint32_t bufferHeight, uint32_t bufferWidth) const noexcept;

			/**
			 * Bind a rasterizing pipeline to the command buffer.
			 *
			 * @param pipeline The pipeline to bind.
			 */
			void bindRasterizingPipeline(VkPipeline pipeline) const noexcept;

			/**
			 * Bind vertex buffers to this command buffer.
			 *
			 * @param vertexStorage The vertex storage to bind.
			 * @param inputs The required inputs.
			 */
			void bindVertexBuffers(const VulkanVertexStorage& vertexStorage, const std::vector<VertexInput>& inputs) const noexcept;

			/**
			 * Bind a n index buffer to this command buffer.
			 *
			 * @param buffer The buffer to bind.
			 */
			void bindIndexBuffer(const VkBuffer buffer) const noexcept;

			/**
			 * Draw using an index buffer.
			 *
			 * @param indexCount The number of indexes to draw.
			 * @param indexOffset The index offset of the mesh.
			 * @param instanceCount The number of instances to draw.
			 * @param vertexOffset The vertex offset to draw in.
			 */
			void drawIndexed(uint64_t indexCount, uint64_t indexOffset, uint64_t instanceCount, uint64_t vertexOffset) const noexcept;

			/**
			 * Bind a graphics descriptor to the command buffer.
			 *
			 * @param pPipeline The pipeline to which the descriptor is bound to.
			 * @param descriptorSet The descriptor set to bind.
			 */
			void bindDescriptor(const VulkanRasterizingPipeline* pPipeline, VkDescriptorSet descriptorSet) const noexcept;

			/**
			 * Execute the current commands on the parent command buffer if available.
			 */
			void execute() const noexcept;

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
			[[nodiscard]] Synchronized<VkCommandBuffer>& getCurrentBuffer() { return m_CurrentCommandBuffer; }

			/**
			 * Get the current command buffer.
			 *
			 * @return The command buffer.
			 */
			[[nodiscard]] const Synchronized<VkCommandBuffer>& getCurrentBuffer() const { return m_CurrentCommandBuffer; }

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

			std::shared_ptr<VulkanCommandBuffers> m_pParent = nullptr;

			VkCommandPool m_CommandPool = VK_NULL_HANDLE;
			Synchronized<VkCommandBuffer> m_CurrentCommandBuffer = VK_NULL_HANDLE;

			uint32_t m_CurrentIndex = 0;

			bool m_IsRecording = false;
		};
	}
}