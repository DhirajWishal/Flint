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
		class VulkanShader;

		namespace Utilities
		{
			void CheckResult(VkResult result);
			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage>& vImages, VkFormat imageFormat, VulkanDevice& device, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, VkImageViewType viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D, UI32 layerCount = 1, UI32 baseLayerIndex = 0, VkComponentMapping mapping = VkComponentMapping());
			VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice vPhysicalDevice);
			bool HasStencilComponent(VkFormat vFormat);
			VkFormat FindDepthFormat(VkPhysicalDevice vPhysicalDevice);
			VkDescriptorType GetDescriptorType(ShaderResourceType type);
			VkShaderStageFlags GetShaderStage(ShaderType type);
			PixelFormat GetPixelFormat(VkFormat format);
			VkFormat GetVulkanFormat(PixelFormat format);
			UI8 GetByteDepth(PixelFormat format);

			void AddResourceBindingsToVector(std::vector<VkDescriptorSetLayoutBinding>& bindings, const VulkanShader& shader);
			void AddPushConstantRangesToVector(std::vector<VkPushConstantRange>& ranges, const VulkanShader& shader);
			void AddPoolSizesToVector(std::vector<VkDescriptorPoolSize>& poolSizes, const VulkanShader& shader);
			void AddResourcesToMap(std::unordered_map<std::string, ShaderResource>& resources, const VulkanShader& shader);
		}
	}
}