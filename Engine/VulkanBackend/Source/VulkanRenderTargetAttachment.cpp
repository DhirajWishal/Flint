// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRenderTargetAttachment.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRenderTargetAttachment::~VulkanRenderTargetAttachment()
		{
			// Destroy the image view.
			destroyImageView();

			// Destroy image.
			destroyImage();
		}

		void VulkanRenderTargetAttachment::createImage(VkImageUsageFlags usageFlags, VkImageTiling tiling)
		{
			// Setup image create info structure.
			VkImageCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.pNext = nullptr;
			imageCreateInfo.flags = 0;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.format = Utility::GetImageFormat(getFormat());
			imageCreateInfo.extent.width = getWidth();
			imageCreateInfo.extent.height = getHeight();
			imageCreateInfo.extent.depth = 1;
			imageCreateInfo.mipLevels = 1;
			imageCreateInfo.arrayLayers = 1;
			imageCreateInfo.samples = Utility::GetSampleCountFlagBits(getMultisample());
			imageCreateInfo.tiling = tiling;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCreateInfo.queueFamilyIndexCount = 0;
			imageCreateInfo.pQueueFamilyIndices = nullptr;
			imageCreateInfo.initialLayout = m_CurrentLayout;
			imageCreateInfo.usage =
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT |	// We might use it to transfer data from this image.
				VK_IMAGE_USAGE_TRANSFER_DST_BIT |	// We might use it to transfer data to this image.
				usageFlags;

			// Setup the allocation info.
			VmaAllocationCreateInfo allocationCreateInfo = {};
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

			// Create the image.
			FLINT_VK_ASSERT(vmaCreateImage(getEngineAs<VulkanEngine>().getAllocator(), &imageCreateInfo, &allocationCreateInfo, &m_Image, &m_Allocation, nullptr), "Failed to create the image!");
		}

		void VulkanRenderTargetAttachment::createImageView(VkImageAspectFlags aspectFlags)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.pNext = nullptr;
			imageViewCreateInfo.flags = 0;
			imageViewCreateInfo.image = m_Image;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = Utility::GetImageFormat(getFormat());
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			FLINT_VK_ASSERT(getEngineAs<VulkanEngine>().getDeviceTable().vkCreateImageView(getEngineAs<VulkanEngine>().getLogicalDevice(), &imageViewCreateInfo, nullptr, &m_ImageView), "Failed to create the image view!");
		}

		void VulkanRenderTargetAttachment::destroyImage()
		{
			vmaDestroyImage(getEngineAs<VulkanEngine>().getAllocator(), m_Image, m_Allocation);
		}

		void VulkanRenderTargetAttachment::destroyImageView()
		{
			getEngineAs<VulkanEngine>().getDeviceTable().vkDestroyImageView(getEngineAs<VulkanEngine>().getLogicalDevice(), m_ImageView, nullptr);
		}

	}
}