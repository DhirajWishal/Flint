// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Rasterizer.hpp"
#include "Core/Containers/SparseArray.hpp"

#include "VulkanDevice.hpp"
#include "VulkanCommandBuffers.hpp"
#include "VulkanRenderTargetAttachment.hpp"
#include "VulkanGraphicsPipeline.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan rasterizer class.
		 */
		class VulkanRasterizer final : public Core::Rasterizer<VulkanDevice, VulkanRenderTargetAttachment>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device reference.
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
			 * @param attachmentDescriptions The attachment descriptions.
			 * @param multisample The multisample count. Default is One.
			 * @param exclusiveBuffering Whether or not to use one buffer/ attachment per frame. Default is false.
			 */
			explicit VulkanRasterizer(VulkanDevice& device, uint32_t width, uint32_t height, uint32_t frameCount, std::vector<Core::AttachmentDescription>&& attachmentDescriptions, Core::Multisample multisample = Core::Multisample::One, bool exclusiveBuffering = false);

			/**
			 * Destructor.
			 */
			~VulkanRasterizer() override;

			/**
			 * Update the render target.
			 * This will perform whatever operation that should be done.
			 */
			void update() override;

			/**
			 * Resize the render target.
			 *
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 */
			void resize(uint32_t width, uint32_t height) override;

			/**
			 * Get the render target attachment at a given index.
			 *
			 * @param index The index of the attachment.
			 * @return The attachment.
			 */
			[[nodiscard]] VulkanRenderTargetAttachment& getAttachment(uint32_t index) override;

			/**
			 * Get the render target attachment at a given index.
			 *
			 * @param index The index of the attachment.
			 * @return The attachment.
			 */
			[[nodiscard]] const VulkanRenderTargetAttachment& getAttachment(uint32_t index) const override;

			/**
			 * Get the render pass.
			 *
			 * @return The render pass.
			 */
			[[nodiscard]] VkRenderPass getRenderPass() const { return m_RenderPass; }

			/**
			 * Get the current frame buffer.
			 *
			 * @return The frame buffer.
			 */
			[[nodiscard]] VkFramebuffer getCurrentFramebuffer() const { return m_Framebuffers[m_FrameIndex]; }

		private:
			/**
			 * Create the attachments.
			 */
			void createAttachments();

			/**
			 * Create the render pass.
			 */
			void createRenderPass();

			/**
			 * Destroy the render pass.
			 */
			void destroyRenderPass();

			/**
			 * Create the frame buffers.
			 */
			void createFramebuffers();

			/**
			 * Destroy the frame buffers.
			 */
			void destroyFramebuffers();

		private:
			std::vector<std::vector<std::unique_ptr<VulkanRenderTargetAttachment>>> m_pAttachments;

			std::vector<VkFramebuffer> m_Framebuffers;
			std::unique_ptr<VulkanCommandBuffers> m_pCommandBuffers = nullptr;

			VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		};
	}
}