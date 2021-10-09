// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanImageView.hpp"
#include "VulkanBackend/VulkanImage.hpp"

#include <optick.h>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanImageView::VulkanImageView(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Image>& pImage, const UI32 baseLayerIndex, const UI32 layerCount, const UI32 baseMipLevel, const UI32 mipLevels, const ImageUsage usage)
			: ImageView(pDevice, pImage, baseLayerIndex, layerCount, baseMipLevel, mipLevels, usage)
		{
			OPTICK_EVENT();
			auto const& vImage = pImage->StaticCast<VulkanImage>();

			VkImageViewCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.viewType = vImage.GetImageViewType();
			vCreateInfo.format = vImage.GetImageFormat();
			vCreateInfo.components = vImage.GetComponentMapping();

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

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateImageView(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vImageView));
		}

		void VulkanImageView::Terminate()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkDestroyImageView(vDevice.GetLogicalDevice(), vImageView, nullptr);
		}
	}
}
