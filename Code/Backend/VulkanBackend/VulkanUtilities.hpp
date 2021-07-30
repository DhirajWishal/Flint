// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"

#include <vulkan/vulkan.h>
#include <vector>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice;

		namespace Utilities
		{
			void CheckResult(VkResult result);
			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage>& vImages, VkFormat imageFormat, VulkanDevice& device, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);
			VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice vPhysicalDevice);
			bool HasStencilComponent(VkFormat vFormat);
			VkFormat FindDepthFormat(VkPhysicalDevice vPhysicalDevice);
			VkDescriptorType GetDescriptorType(ShaderResourceType type);
		}
	}
}