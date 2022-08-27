// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/RenderTargetAttachment.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace Backend
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
			 * @param pDevice The device reference.
			 * @param width The width of the attachment.
			 * @param height The height of the attachment.
			 * @param type The type of the attachment.
			 * @param format The format of the attachment.
			 * @param multisample The multisample count to use.
			 */
			explicit VulkanRenderTargetAttachment(const std::shared_ptr<VulkanDevice>& pDevice, uint32_t width, uint32_t height, AttachmentType type, PixelFormat format, Multisample multisample)
				: RenderTargetAttachment(pDevice, width, height, type, format, multisample) {}

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
			 * Get the image.
			 *
			 * @return The image.
			 */
			[[nodiscard]] VkImage getImage() const { return m_Image; }

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
			 * @param tiling The image tiling.
			 */
			void createImage(VkImageUsageFlags usageFlags, VkImageTiling tiling);

			/**
			 * Create the image view.
			 *
			 * @param aspectFlags The image view aspect flags.
			 */
			void createImageView(VkImageAspectFlags aspectFlags);

			/**
			 * Clear the internal contents.
			 */
			void clear();

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