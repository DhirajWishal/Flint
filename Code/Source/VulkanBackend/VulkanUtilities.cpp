// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanUtilities.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanDevice.hpp"
#include "VulkanBackend/VulkanShader.hpp"

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

			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage>& vImages, VkFormat imageFormat, VulkanDevice& device, VkImageAspectFlags aspectFlags, VkImageViewType viewType, UI32 layerCount, UI32 baseLayerIndex, VkComponentMapping mapping)
			{
				VkImageViewCreateInfo vCreateInfo = {};
				vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				vCreateInfo.flags = 0;
				vCreateInfo.pNext = VK_NULL_HANDLE;
				vCreateInfo.viewType = viewType;
				vCreateInfo.format = imageFormat;
				vCreateInfo.components = mapping;

				VkImageSubresourceRange vSR = {};
				vSR.layerCount = layerCount;
				vSR.baseArrayLayer = baseLayerIndex;
				vSR.levelCount = 1;
				vSR.baseMipLevel = 0;
				vSR.aspectMask = aspectFlags;

				vCreateInfo.subresourceRange = vSR;

				std::vector<VkImageView> vImageViews(vImages.size());
				VkImageView* pArray = vImageViews.data();
				for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
				{
					vCreateInfo.image = *itr;
					FLINT_VK_ASSERT(vkCreateImageView(device.GetLogicalDevice(), &vCreateInfo, nullptr, pArray));
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
				case Flint::ShaderResourceType::SAMPLER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;

				case Flint::ShaderResourceType::COMBINED_IMAGE_SAMPLER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

				case Flint::ShaderResourceType::SAMPLED_IMAGE:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

				case Flint::ShaderResourceType::STORAGE_IMAGE:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

				case Flint::ShaderResourceType::UNIFORM_TEXEL_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;

				case Flint::ShaderResourceType::STORAGE_TEXEL_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;

				case Flint::ShaderResourceType::UNIFORM_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

				case Flint::ShaderResourceType::STORAGE_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

				case Flint::ShaderResourceType::UNIFORM_BUFFER_DYNAMIC:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

				case Flint::ShaderResourceType::STORAGE_BUFFER_DYNAMIC:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

				case Flint::ShaderResourceType::INPUT_ATTACHMENT:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

				case Flint::ShaderResourceType::ACCELERATION_STRUCTURE:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid or undefined shader resource type!");
				}

				return VkDescriptorType();
			}

			VkShaderStageFlags GetShaderStage(ShaderType type)
			{
				switch (type)
				{
				case Flint::ShaderType::VERTEX:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

				case Flint::ShaderType::TESSELLATION_CONTROL:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

				case Flint::ShaderType::TESSELLATION_EVALUATION:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

				case Flint::ShaderType::GEOMETRY:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;

				case Flint::ShaderType::FRAGMENT:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;

				case Flint::ShaderType::COMPUTE:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;

				case Flint::ShaderType::RAY_GEN:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_RAYGEN_BIT_KHR;

				case Flint::ShaderType::ANY_HIT:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

				case Flint::ShaderType::CLOSEST_HIT:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

				case Flint::ShaderType::RAY_MISS:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_MISS_BIT_KHR;

				default:
					FLINT_THROW_RUNTIME_ERROR("Invalid or Undefined shader type!");
				}

				return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
			}

			PixelFormat GetPixelFormat(VkFormat format)
			{
				switch (format)
				{
				case VK_FORMAT_UNDEFINED:
					return PixelFormat::UNDEFINED;

				case VK_FORMAT_R8_SRGB:
					return PixelFormat::R8_SRGB;

				case VK_FORMAT_R8G8_SRGB:
					return PixelFormat::R8G8_SRGB;

				case VK_FORMAT_R8G8B8_SRGB:
					return PixelFormat::R8G8B8_SRGB;

				case VK_FORMAT_R8G8B8A8_SRGB:
					return PixelFormat::R8G8B8A8_SRGB;

				case VK_FORMAT_R8_UNORM:
					return PixelFormat::R8_UNORMAL;

				case VK_FORMAT_R8G8_UNORM:
					return PixelFormat::R8G8_UNORMAL;

				case VK_FORMAT_R8G8B8_UNORM:
					return PixelFormat::R8G8B8_UNORMAL;

				case VK_FORMAT_R8G8B8A8_UNORM:
					return PixelFormat::R8G8B8A8_UNORMAL;

				case VK_FORMAT_B8G8R8_SRGB:
					return PixelFormat::B8G8R8_SRGB;

				case VK_FORMAT_B8G8R8A8_SRGB:
					return PixelFormat::B8G8R8A8_SRGB;

				case VK_FORMAT_B8G8R8_UNORM:
					return PixelFormat::B8G8R8_UNORMAL;

				case VK_FORMAT_B8G8R8A8_UNORM:
					return PixelFormat::B8G8R8A8_UNORMAL;

				case VK_FORMAT_R16_SFLOAT:
					return PixelFormat::R16_SFLOAT;

				case VK_FORMAT_R16G16_SFLOAT:
					return PixelFormat::R16G16_SFLOAT;

				case VK_FORMAT_R16G16B16_SFLOAT:
					return PixelFormat::R16G16B16_SFLOAT;

				case VK_FORMAT_R16G16B16A16_SFLOAT:
					return PixelFormat::R16G16B16A16_SFLOAT;

				case VK_FORMAT_R32_SFLOAT:
					return PixelFormat::R32_SFLOAT;

				case VK_FORMAT_R32G32_SFLOAT:
					return PixelFormat::R32G32_SFLOAT;

				case VK_FORMAT_R32G32B32_SFLOAT:
					return PixelFormat::R32G32B32_SFLOAT;

				case VK_FORMAT_R32G32B32A32_SFLOAT:
					return PixelFormat::R32G32B32A32_SFLOAT;

				case VK_FORMAT_D16_UNORM:
					return PixelFormat::D16_SINT;

				case VK_FORMAT_D32_SFLOAT:
					return PixelFormat::D32_SFLOAT;

				default:
					FLINT_THROW_BACKEND_ERROR("Unsupported format!");
				}

				return PixelFormat::UNDEFINED;
			}

			VkFormat GetVulkanFormat(PixelFormat format)
			{
				switch (format)
				{
				case Flint::PixelFormat::UNDEFINED:
					return VkFormat::VK_FORMAT_UNDEFINED;

				case Flint::PixelFormat::R8_SRGB:
					return VkFormat::VK_FORMAT_R8_SRGB;

				case Flint::PixelFormat::R8G8_SRGB:
					return VkFormat::VK_FORMAT_R8G8_SRGB;

				case Flint::PixelFormat::R8G8B8_SRGB:
					return VkFormat::VK_FORMAT_R8G8B8_SRGB;

				case Flint::PixelFormat::R8G8B8A8_SRGB:
					return VkFormat::VK_FORMAT_R8G8B8A8_SRGB;

				case Flint::PixelFormat::R8_UNORMAL:
					return VkFormat::VK_FORMAT_R8_UNORM;

				case Flint::PixelFormat::R8G8_UNORMAL:
					return VkFormat::VK_FORMAT_R8G8_UNORM;

				case Flint::PixelFormat::R8G8B8_UNORMAL:
					return VkFormat::VK_FORMAT_R8G8B8_UNORM;

				case Flint::PixelFormat::R8G8B8A8_UNORMAL:
					return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;

				case Flint::PixelFormat::B8G8R8_SRGB:
					return VkFormat::VK_FORMAT_B8G8R8_SRGB;

				case Flint::PixelFormat::B8G8R8A8_SRGB:
					return VkFormat::VK_FORMAT_B8G8R8A8_SRGB;

				case Flint::PixelFormat::B8G8R8_UNORMAL:
					return VkFormat::VK_FORMAT_B8G8R8_UNORM;

				case Flint::PixelFormat::B8G8R8A8_UNORMAL:
					return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;

				case Flint::PixelFormat::R16_SFLOAT:
					return VkFormat::VK_FORMAT_R16_SFLOAT;

				case Flint::PixelFormat::R16G16_SFLOAT:
					return VkFormat::VK_FORMAT_R16G16_SFLOAT;

				case Flint::PixelFormat::R16G16B16_SFLOAT:
					return VkFormat::VK_FORMAT_R16G16B16_SFLOAT;

				case Flint::PixelFormat::R16G16B16A16_SFLOAT:
					return VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT;

				case Flint::PixelFormat::R32_SFLOAT:
					return VkFormat::VK_FORMAT_R32_SFLOAT;

				case Flint::PixelFormat::R32G32_SFLOAT:
					return VkFormat::VK_FORMAT_R32G32_SFLOAT;

				case Flint::PixelFormat::R32G32B32_SFLOAT:
					return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;

				case Flint::PixelFormat::R32G32B32A32_SFLOAT:
					return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;

				case Flint::PixelFormat::D16_SINT:
					return VkFormat::VK_FORMAT_D16_UNORM;

				case Flint::PixelFormat::D32_SFLOAT:
					return VkFormat::VK_FORMAT_D32_SFLOAT;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid pixel format!");
				}

				return VkFormat::VK_FORMAT_UNDEFINED;
			}

			UI8 GetByteDepth(PixelFormat format)
			{
				switch (format)
				{
				case Flint::PixelFormat::R8_SRGB:
				case Flint::PixelFormat::R8_UNORMAL:
					return 1;

				case Flint::PixelFormat::R8G8_SRGB:
				case Flint::PixelFormat::R8G8_UNORMAL:
				case Flint::PixelFormat::D16_SINT:
				case Flint::PixelFormat::R16_SFLOAT:
					return 2;

				case Flint::PixelFormat::R8G8B8_SRGB:
				case Flint::PixelFormat::R8G8B8_UNORMAL:
				case Flint::PixelFormat::B8G8R8_SRGB:
				case Flint::PixelFormat::B8G8R8_UNORMAL:
					return 3;

				case Flint::PixelFormat::R8G8B8A8_SRGB:
				case Flint::PixelFormat::R8G8B8A8_UNORMAL:
				case Flint::PixelFormat::B8G8R8A8_SRGB:
				case Flint::PixelFormat::B8G8R8A8_UNORMAL:
				case Flint::PixelFormat::D32_SFLOAT:
				case Flint::PixelFormat::R32_SFLOAT:
				case Flint::PixelFormat::R16G16_SFLOAT:
					return 4;

				case Flint::PixelFormat::R16G16B16_SFLOAT:
					return 6;

				case Flint::PixelFormat::R32G32_SFLOAT:
				case Flint::PixelFormat::R16G16B16A16_SFLOAT:
					return 8;

				case Flint::PixelFormat::R32G32B32_SFLOAT:
					return 12;

				case Flint::PixelFormat::R32G32B32A32_SFLOAT:
					return 16;
				}

				return 0;
			}

			void AddResourceBindingsToVector(std::vector<VkDescriptorSetLayoutBinding>& bindings, const VulkanShader& shader)
			{
				std::vector<VkDescriptorSetLayoutBinding> tempBindings = shader.GetResourceBindings();
				bindings.insert(bindings.end(), tempBindings.begin(), tempBindings.end());
			}

			void AddPushConstantRangesToVector(std::vector<VkPushConstantRange>& ranges, const VulkanShader& shader)
			{
				std::vector<VkPushConstantRange> tempRanges = shader.GetPushConstantRanges();
				ranges.insert(ranges.end(), tempRanges.begin(), tempRanges.end());
			}

			void AddPoolSizesToVector(std::vector<VkDescriptorPoolSize>& poolSizes, const VulkanShader& shader)
			{
				std::vector<VkDescriptorPoolSize> tempPoolSizes = shader.GetPoolSizes();
				poolSizes.insert(poolSizes.end(), tempPoolSizes.begin(), tempPoolSizes.end());
			}

			void AddResourcesToMap(std::unordered_map<std::string, ShaderResource>& resources, const VulkanShader& shader)
			{
				const std::unordered_map<std::string, ShaderResource> tempResources = shader.GetShaderResources();
				resources.insert(tempResources.begin(), tempResources.end());
			}
		}
	}
}