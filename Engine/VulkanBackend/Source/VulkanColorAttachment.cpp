// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanColorAttachment.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanColorAttachment::VulkanColorAttachment(VulkanDevice& device, uint32_t width, uint32_t height, Core::PixelFormat format, Core::Multisample multisample)
			: VulkanRenderTargetAttachment(device, width, height, Core::AttachmentType::Color, format, multisample)
		{
			// Create the image.
			createImage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_TILING_OPTIMAL);

			// Create the image view.
			createImageView(VK_IMAGE_ASPECT_COLOR_BIT);

			// Make sure to set the object as valid.
			validate();
		}

		VulkanColorAttachment::~VulkanColorAttachment()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanColorAttachment::terminate()
		{
			clear();
			invalidate();
		}

		VkAttachmentDescription VulkanColorAttachment::getAttachmentDescription() const
		{
			VkAttachmentDescription attachmentDescription = {};
			attachmentDescription.flags = 0;
			attachmentDescription.format = Utility::GetImageFormat(getFormat());
			attachmentDescription.samples = Utility::GetSampleCountFlagBits(getMultisample());
			attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			return attachmentDescription;
		}

		VkImageLayout VulkanColorAttachment::getLayout() const
		{
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	}
}