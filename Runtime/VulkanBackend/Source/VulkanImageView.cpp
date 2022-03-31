// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanImageView.hpp"
#include "VulkanBackend/VulkanImage.hpp"

#include <optick.h>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanImageView::VulkanImageView(VulkanDevice* m_pDevice, const VulkanImage* pImage, const uint32_t baseLayerIndex, const uint32_t layerCount, const uint32_t baseMipLevel, const uint32_t mipLevels, const ImageUsage usage)
			: ImageView(m_pDevice, pImage, baseLayerIndex, layerCount, baseMipLevel, mipLevels, usage)
		{
			OPTICK_EVENT();

			VkImageViewCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.image = pImage->GetImage();
			m_vCreateInfo.viewType = pImage->GetImageViewType();
			m_vCreateInfo.format = pImage->GetImageFormat();
			m_vCreateInfo.components = pImage->GetComponentMapping();

			m_vCreateInfo.subresourceRange.layerCount = layerCount;
			m_vCreateInfo.subresourceRange.baseArrayLayer = baseLayerIndex;
			m_vCreateInfo.subresourceRange.levelCount = mipLevels;
			m_vCreateInfo.subresourceRange.baseMipLevel = baseMipLevel;
			m_vCreateInfo.subresourceRange.aspectMask = Utilities::GetImageAspectFlags(usage);

			switch (m_vCreateInfo.subresourceRange.aspectMask)
			{
			case VK_IMAGE_ASPECT_COLOR_BIT:
				m_vCreateInfo.components = { };
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

			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkCreateImageView(m_pDevice->GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vImageView));
		}

		void VulkanImageView::Terminate()
		{
			m_pDevice->GetDeviceTable().vkDestroyImageView(m_pDevice->GetLogicalDevice(), m_vImageView, nullptr);
			bIsTerminated = true;
		}
	}
}
