// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Core/Error.hpp"
#include "VulkanMacros.hpp"
#include "VulkanDevice.hpp"

#include <vulkan/vulkan.h>
#include <vector>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice;

		namespace Utilities
		{
			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage> &vImages, VkFormat imageFormat, VulkanDevice &device, VkImageAspectFlags aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT)
			{
				VkImageViewCreateInfo vCI = {};
				vCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				vCI.flags = 0;
				vCI.pNext = VK_NULL_HANDLE;
				vCI.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
				vCI.format = imageFormat;

				VkImageSubresourceRange vSR = {};
				vSR.layerCount = 1;
				vSR.baseArrayLayer = 0;
				vSR.levelCount = 1;
				vSR.baseMipLevel = 0;
				vSR.aspectMask = aspectFlags;

				vCI.subresourceRange = vSR;

				std::vector<VkImageView> vImageViews(vImages.size());
				VkImageView *pArray = vImageViews.data();
				for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
				{
					vCI.image = *itr;
					FLINT_VK_ASSERT(vkCreateImageView(device.GetLogicalDevice(), &vCI, nullptr, pArray));
					pArray++;
				}

				return vImageViews;
			}

			VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice vPhysicalDevice)
			{
				for (VkFormat format : candidates)
				{
					VkFormatProperties props = {};
					vkGetPhysicalDeviceFormatProperties(vPhysicalDevice, format, &props);

					if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
						return format;

					else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
						return format;
				}

				FLINT_THROW_RUNTIME_ERROR("Unable to find suitable format!");
				return VkFormat();
			}

			bool HasStencilComponent(VkFormat vFormat)
			{
				return vFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || vFormat == VK_FORMAT_D24_UNORM_S8_UINT;
			}

			VkFormat FindDepthFormat(VkPhysicalDevice vPhysicalDevice)
			{
				return FindSupportedFormat(
					{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
					VK_IMAGE_TILING_OPTIMAL,
					VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
					vPhysicalDevice);
			}
		}
	}
}