// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan color attachment class.
		 * This contains the color image and other utility methods.
		 */
		class VulkanColorAttachment final : public VulkanRenderTargetAttachment
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device reference.
			 * @param width The width of the attachment.
			 * @param height The height of the attachment.
			 * @param format The format of the attachment.
			 * @param multisample The multisample count to use.
			 */
			explicit VulkanColorAttachment(const std::shared_ptr<VulkanDevice>& pDevice, uint32_t width, uint32_t height, Core::PixelFormat format, Core::Multisample multisample);

			/**
			 * Destructor.
			 */
			~VulkanColorAttachment() override;

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