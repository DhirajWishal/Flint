// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Vulkan storage attachment class.
		 * This contains the color image and other utility methods used by the ray tracer.
		 */
		class VulkanStorageAttachment final : public VulkanRenderTargetAttachment
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device reference.
			 * @param width The width of the attachment.
			 * @param height The height of the attachment.
			 * @param format The format of the attachment.
			 */
			explicit VulkanStorageAttachment(const std::shared_ptr<VulkanDevice>& pDevice, uint32_t width, uint32_t height, PixelFormat format);

			/**
			 * Destructor.
			 */
			~VulkanStorageAttachment() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

			/**
			 * Get the attachment description.
			 *
			 * @return The attachment description.
			 */
			[[nodiscard]] VkAttachmentDescription getAttachmentDescription() const override;

			/**
			 * Get the attachment layout.
			 *
			 * @return The attachment layout.
			 */
			[[nodiscard]] VkImageLayout getLayout() const override;
		};
	}
}