// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanDepthAttachment.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanDepthAttachment::VulkanDepthAttachment(const std::shared_ptr<VulkanDevice>& pDevice, uint32_t width, uint32_t height, Core::PixelFormat format, Core::Multisample multisample)
			: VulkanRenderTargetAttachment(pDevice, width, height, Core::AttachmentType::Depth, format, multisample)
		{
			// Create the image.
			createImage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_TILING_OPTIMAL);

			// Create the image view.
			createImageView(VK_IMAGE_ASPECT_DEPTH_BIT);

			// Make sure to set the object as valid.
			validate();
		}

		VulkanDepthAttachment::~VulkanDepthAttachment()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanDepthAttachment::terminate()
		{
			clear();
			invalidate();
		}

		VkAttachmentDescription VulkanDepthAttachment::getAttachmentDescription() const
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
			attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

			return attachmentDescription;
		}

		VkImageLayout VulkanDepthAttachment::getLayout() const
		{
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
	}
}