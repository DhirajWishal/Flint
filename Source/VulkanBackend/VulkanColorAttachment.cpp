// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/VulkanBackend/VulkanColorAttachment.hpp"
#include "Flint/VulkanBackend/VulkanMacros.hpp"

#include <Optick.h>

namespace Flint
{
	namespace Backend
	{
		VulkanColorAttachment::VulkanColorAttachment(const std::shared_ptr<VulkanDevice>& pDevice, uint32_t width, uint32_t height, PixelFormat format, Multisample multisample)
			: VulkanRenderTargetAttachment(pDevice, width, height, AttachmentType::Color, format, multisample)
		{
			OPTICK_EVENT();

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
			OPTICK_EVENT();

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