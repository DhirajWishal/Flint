// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan depth attachment class.
		 * This contains the depth image and other utility methods.
		 */
		class VulkanDepthAttachment final : public VulkanRenderTargetAttachment
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param engine The engine reference.
			 * @param width The width of the attachment.
			 * @param height The height of the attachment.
			 * @param format The format of the attachment.
			 * @param multisample The multisample count to use.
			 */
			explicit VulkanDepthAttachment(Engine& engine, uint32_t width, uint32_t height, PixelFormat format, Multisample multisample);

			/**
			 * Destructor.
			 */
			~VulkanDepthAttachment() override;

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