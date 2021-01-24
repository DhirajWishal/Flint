// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.h"
#include "Core/Types/DataTypes.h"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Utilities
		{
			VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, UI32 width, UI32 height);
			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage>& vImages, VkFormat imageFormat, VulkanDevice& device, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);
			VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice vPhysicalDevice);
			VkFormat FindDepthFormat(VkPhysicalDevice vPhysicalDevice);
			bool HasStencilComponent(VkFormat vFormat);
		}
	}
}