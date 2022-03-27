// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanImageView.hpp"
#include "VulkanBackend/VulkanImage.hpp"

#include <optick.h>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanImageView::VulkanImageView(VulkanDevice* pDevice, const VulkanImage* pImage, const uint32_t baseLayerIndex, const uint32_t layerCount, const uint32_t baseMipLevel, const uint32_t mipLevels, const ImageUsage usage)
			: ImageView(pDevice, pImage, baseLayerIndex, layerCount, baseMipLevel, mipLevels, usage)
		{
			OPTICK_EVENT();

			VkImageViewCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.image = pImage->GetImage();
			vCreateInfo.viewType = pImage->GetImageViewType();
			vCreateInfo.format = pImage->GetImageFormat();
			vCreateInfo.components = pImage->GetComponentMapping();

			vCreateInfo.subresourceRange.layerCount = layerCount;
			vCreateInfo.subresourceRange.baseArrayLayer = baseLayerIndex;
			vCreateInfo.subresourceRange.levelCount = mipLevels;
			vCreateInfo.subresourceRange.baseMipLevel = baseMipLevel;
			vCreateInfo.subresourceRange.aspectMask = Utilities::GetImageAspectFlags(usage);

			switch (vCreateInfo.subresourceRange.aspectMask)
			{
			case VK_IMAGE_ASPECT_COLOR_BIT:
				vCreateInfo.components = { };
				break;

			case VK_IMAGE_ASPECT_DEPTH_BIT:
				break;

			case VK_IMAGE_ASPECT_STENCIL_BIT:
				break;

			case VK_IMAGE_ASPECT_METADATA_BIT:
				break;

			case VK_IMAGE_ASPECT_PLANE_0_BIT:
				break;

			case VK_IMAGE_ASPECT_PLANE_1_BIT:
				break;

			case VK_IMAGE_ASPECT_PLANE_2_BIT:
				break;

			case VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT:
				break;

			case VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM:
				break;

			default:
				throw backend_error("Unsupported usage flag!");
			}

			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreateImageView(pDevice->GetLogicalDevice(), &vCreateInfo, nullptr, &vImageView));
		}

		void VulkanImageView::Terminate()
		{
			pDevice->GetDeviceTable().vkDestroyImageView(pDevice->GetLogicalDevice(), vImageView, nullptr);
			bIsTerminated = true;
		}
	}
}
