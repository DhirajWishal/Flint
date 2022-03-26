// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Shader.hpp"

#include <volk.h>
#include <vector>

#include <optick.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice;
		class VulkanShader;

		namespace Utilities
		{
			void CheckResult(VkResult result);
			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage>& vImages, VkFormat imageFormat, VulkanDevice& device, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, VkImageViewType viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D, uint32 layerCount = 1, uint32 baseLayerIndex = 0, uint32 mipLevels = 1, uint32 baseMipLevel = 0, VkComponentMapping mapping = VkComponentMapping());
			VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice vPhysicalDevice);
			bool HasStencilComponent(VkFormat vFormat);
			VkFormat FindDepthFormat(VkPhysicalDevice vPhysicalDevice);
			VkDescriptorType GetDescriptorType(ShaderResourceType type);
			VkShaderStageFlags GetShaderStage(const ShaderType type);
			PixelFormat GetPixelFormat(VkFormat format);
			VkFormat GetVulkanFormat(const PixelFormat format);
			uint8 GetByteDepth(const PixelFormat format);
			VkSampleCountFlagBits GetSampleCount(MultiSampleCount count);
			VkImageAspectFlags GetImageAspectFlags(const ImageUsage usage);

			VkAccessFlags GetAccessFlags(VkImageLayout layout);
			VkPipelineStageFlags GetPipelineStageFlags(VkAccessFlags flags);

			void AddPushConstantRangesToVector(std::vector<VkPushConstantRange>& ranges, const VulkanShader& shader);
			void AddResourcesToMap(TShaderResourceMap& resources, const VulkanShader& shader);
		}
	}
}