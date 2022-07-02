// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanTexture2DView.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanTexture2DView::VulkanTexture2DView(const std::shared_ptr<VulkanDevice>& pDevice, const std::shared_ptr<VulkanTexture2D>& pTexture)
			: TextureView(pDevice, pTexture)
		{
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.format = Utility::GetImageFormat(pTexture->getPixelFormat());
			createInfo.image = pTexture->getImageHandle();
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.layerCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.levelCount = pTexture->getMipLevels();
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

			FLINT_VK_ASSERT(pDevice->getDeviceTable().vkCreateImageView(pDevice->getLogicalDevice(), &createInfo, nullptr, &m_ImageView), "Failed to create the image view!");

			validate();
		}

		VulkanTexture2DView::~VulkanTexture2DView()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanTexture2DView::terminate()
		{
			getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyImageView(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_ImageView, nullptr);
			invalidate();
		}
	}
}