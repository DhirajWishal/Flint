// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Rasterizer.hpp"
#include "VulkanEngine.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan rasterizer class.
		 */
		class VulkanRasterizer final : public Rasterizer
		{
			/**
			 * Explicit constructor.
			 *
			 * @param engine The engine reference.
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
			 * @param attachmentDescriptions The attachment descriptions.
			 * @param multisample The multisample count. Default is One.
			 */
			explicit VulkanRasterizer(Engine& engine, uint32_t width, uint32_t height, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One);

			/**
			 * Destructor.
			 */
			~VulkanRasterizer() override;

		private:
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
			std::vector<VkFramebuffer> m_Framebuffers;

			VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		};
	}
}