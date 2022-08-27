// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Window.hpp"
#include "VulkanCommandBuffers.hpp"

#include <SDL.h>

namespace Flint
{
	namespace Backend
	{
		class VulkanRenderTargetAttachment;

		/**
		 * Vulkan window class.
		 */
		class VulkanWindow final : public Window
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device to which this window is bound to.
			 * @param title The window title.
			 * @param width The width of the window. If set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
			 * @param height The height of the window. Same as the width, if set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
			 */
			explicit VulkanWindow(const std::shared_ptr<VulkanDevice>& pDevice, std::string&& title, uint32_t width = -1, uint32_t height = -1);

			/**
			 * Destructor.
			 */
			~VulkanWindow() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

			/**
			 * Update the window and all the graphics.
			 * This will process all the nodes and will send the final image to the surface.
			 */
			void update() override;

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
			 * Get the best buffer count.
			 *
			 * @return The buffer count.
			 */
			[[nodiscard]] uint32_t getBestBufferCount() const;

			/**
			 * Refresh the extent by getting the latest window extent.
			 * This might be needed when the window was resized.
			 */
			void refreshExtent();

			/**
			 * Create the window surface.
			 */
			void createSurface();

			/**
			 * Destroy the window surface.
			 */
			void destroySurface();

			/**
			 * Clear the swapchain.
			 */
			void clearSwapchain();

			/**
			 * Resolve the image views.
			 */
			void resolveImageViews();

			/**
			 * Create the swapchain.
			 */
			void createSwapchain();

			/**
			 * Create the sync objects.
			 */
			void createSyncObjects();

			/**
			 * Destroy the sync objects.
			 */
			void destroySyncObjects();

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

			/**
			 * Recreate the whole window.
			 * This is needed when the screen was resized.
			 */
			void recreate();

			/**
			 * Copy the dependency and submit the frame to the GPU.
			 */
			void copyAndSubmitFrame();

			/**
			 * Present the images to the user.
			 */
			void present();

		private:
			std::unique_ptr<VulkanCommandBuffers> m_pCommandBuffers = nullptr;

			std::vector<VkImage> m_SwapchainImages;
			std::vector<VkImageView> m_SwapchainImageViews;

			std::vector<VkSemaphore> m_InFlightSemaphores;
			std::vector<VkSemaphore> m_RenderFinishedSemaphores;

			std::vector<VkFramebuffer> m_Framebuffers;

			SDL_Window* m_pWindow = nullptr;

			VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
			VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

			VkRenderPass m_RenderPass = VK_NULL_HANDLE;

			VkFormat m_SwapchainFormat = VK_FORMAT_UNDEFINED;

			uint32_t m_ImageIndex = 0;

			bool m_FirstTime = true;
			bool m_IsMinimized = false;
		};
	}
}