// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "core/RenderTargetAttachment.hpp"
#include "VulkanEngine.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan depth attachment class.
		 * This contains the depth image and other utility methods.
		 */
		class VulkanRenderTargetAttachment : public RenderTargetAttachment
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param engine The engine reference.
			 * @param width The width of the attachment.
			 * @param height The height of the attachment.
			 * @param type The type of the attachment.
			 * @param format The format of the attachment.
			 * @param multisample The multisample count to use.
			 */
			explicit VulkanRenderTargetAttachment(Engine& engine, uint32_t width, uint32_t height, AttachmentType type, PixelFormat format, Multisample multisample)
				: RenderTargetAttachment(engine, width, height, type, format, multisample) {}

			/**
			 * Virtual destructor.
			 */
			virtual ~VulkanRenderTargetAttachment() override;

			/**
			 * Get the attachment description.
			 *
			 * @return The attachment description.
			 */
			[[nodiscard]] virtual VkAttachmentDescription getAttachmentDescription() const = 0;

			/**
			 * Get the attachment layout.
			 *
			 * @return The attachment layout.
			 */
			[[nodiscard]] virtual VkImageLayout getLayout() const = 0;

			/**
			 * Get the image view.
			 *
			 * @return The image view.
			 */
			[[nodiscard]] VkImageView getImageView() const { return m_ImageView; }

		protected:
			/**
			 * Create the image.
			 *
			 * @param usageFlags The image usage flags.
			 */
			void createImage(VkImageUsageFlags usageFlags);

			/**
			 * Create the image view.
			 *
			 * @param aspectFlags The image view aspect flags.
			 */
			void createImageView(VkImageAspectFlags aspectFlags);

		private:
			/**
			 * Destroy the image.
			 */
			void destroyImage();

			/**
			 * Destroy the image view.
			 */
			void destroyImageView();

		private:
			VkImage m_Image = VK_NULL_HANDLE;
			VkImageView m_ImageView = VK_NULL_HANDLE;

			VmaAllocation m_Allocation = nullptr;
			VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		};
	}
}