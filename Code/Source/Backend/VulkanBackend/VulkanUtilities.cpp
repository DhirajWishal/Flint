// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanUtilities.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Utilities
		{
			void CheckResult(VkResult result)
			{
				switch (result)
				{
				case VK_SUCCESS:										return;
				case VK_NOT_READY:										FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_DEVICE_NOT_READY);
				case VK_TIMEOUT:										FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_DEVICE_TIMEOUT);
				case VK_EVENT_SET:										FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_EVENT_SET);
				case VK_EVENT_RESET:									FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_EVENT_RESET);
				case VK_INCOMPLETE:										FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_INCOMPLETE_COMMAND);
				case VK_ERROR_OUT_OF_HOST_MEMORY:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_DEVICE_OUT_OF_HOST_MEMORY);
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_DEVICE_OUT_OF_DEVICE_MEMORY);
				case VK_ERROR_INITIALIZATION_FAILED:					FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_INITIALIZATION_FAILED);
				case VK_ERROR_DEVICE_LOST:								FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_DEVICE_LOST);
				case VK_ERROR_MEMORY_MAP_FAILED:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_MEMORY_MAPPING_FAILED);
				case VK_ERROR_LAYER_NOT_PRESENT:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_VULKAN_LAYER_NOT_PRESENT);
				case VK_ERROR_EXTENSION_NOT_PRESENT:					FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_VULKAN_EXTENSION_NOT_PRESENT);
				case VK_ERROR_FEATURE_NOT_PRESENT:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_DEVICE_FEATURE_NOT_PRESENT);
				case VK_ERROR_INCOMPATIBLE_DRIVER:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_INCOMPATIBLE_DRIVER);
				case VK_ERROR_TOO_MANY_OBJECTS:							FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_TOO_MANY_OBJECTS);
				case VK_ERROR_FORMAT_NOT_SUPPORTED:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_FORMAT_NOT_SUPPORTED);
				case VK_ERROR_FRAGMENTED_POOL:							FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_FRAGMENTED_POOL);
				case VK_ERROR_OUT_OF_POOL_MEMORY:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_OUT_OF_POOL_MEMORY);
				case VK_ERROR_INVALID_EXTERNAL_HANDLE:					FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_INVALID_HANDLE);
				case VK_ERROR_FRAGMENTATION:							FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_INTERNAL_FRAGMENTATION);
				case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:			FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_VULKAN_INVALID_OPAQUE_CAPTURE_ADDRESS);
				case VK_ERROR_SURFACE_LOST_KHR:							FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_DISPLAY_LOST);
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:					FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_DISPLAY_IN_USE);
				case VK_SUBOPTIMAL_KHR:									FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_RENDER_TARGET_SUBOPTIMAL);
				case VK_ERROR_OUT_OF_DATE_KHR:							FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_RENDER_TARGET_OUT_OF_DATE);
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:					FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_INCOMPATIBLE_DISPLAY);
				case VK_ERROR_VALIDATION_FAILED_EXT:					FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_VALIDATION_FAILED);
				case VK_ERROR_INVALID_SHADER_NV:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_INVALID_SHADER);
				case VK_ERROR_NOT_PERMITTED_EXT:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_OPERATION_NOT_PERMITTED);
				case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:		FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_FULL_SCREEN_MODE_LOST);
				case VK_THREAD_IDLE_KHR:								FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_DEVICE_THREAD_IDLE);
				case VK_THREAD_DONE_KHR:								FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_DEVICE_THREAD_DONE);
				case VK_OPERATION_DEFERRED_KHR:							FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_OPERATION_DEFERRED);
				case VK_OPERATION_NOT_DEFERRED_KHR:						FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_OPERATION_NOT_DEFERRED);
				case VK_PIPELINE_COMPILE_REQUIRED_EXT:					FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_PIPELINE_COMPILATION_REQUIRED);
				default:												FLINT_THROW_BACKEND_ERROR(FLINT_BACKEND_ERROR_UNKNOWN);
				}
			}

			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage>& vImages, VkFormat imageFormat, VulkanDevice& device, VkImageAspectFlags aspectFlags)
			{
				VkImageViewCreateInfo vCI = {};
				vCI.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
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
				VkImageView* pArray = vImageViews.data();
				for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
				{
					vCI.image = *itr;
					FLINT_VK_ASSERT(vkCreateImageView(device.GetLogicalDevice(), &vCI, nullptr, pArray));
					pArray++;
				}

				return vImageViews;
			}

			VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice vPhysicalDevice)
			{
				for (VkFormat format : candidates) {
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
					{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
					VK_IMAGE_TILING_OPTIMAL,
					VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
					vPhysicalDevice
				);
			}

			VkDescriptorType GetDescriptorType(ShaderResourceType type)
			{
				switch (type)
				{
				case Flint::ShaderResourceType::UNIFORM_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

				case Flint::ShaderResourceType::STORAGE_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

				case Flint::ShaderResourceType::SAMPLER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid or undefined shader resource type!");
				}

				return VkDescriptorType();
			}
		}
	}
}