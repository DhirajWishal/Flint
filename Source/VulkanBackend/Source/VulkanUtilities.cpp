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
				case VkResult::VK_SUCCESS:											return;
				case VkResult::VK_NOT_READY:										throw backend_error(BackendErrorDeviceNotReady);
				case VkResult::VK_TIMEOUT:											throw backend_error(BackendErrorDeviceTimeout);
				case VkResult::VK_EVENT_SET:										throw backend_error(BackendErrorEventSet);
				case VkResult::VK_EVENT_RESET:										throw backend_error(BackendErrorEventReset);
				case VkResult::VK_INCOMPLETE:										throw backend_error(BackendErrorIncompleteCommand);
				case VkResult::VK_ERROR_OUT_OF_HOST_MEMORY:							throw backend_error(BackendErrorDeviceOutOfHostMemory);
				case VkResult::VK_ERROR_OUT_OF_DEVICE_MEMORY:						throw backend_error(BackendErrorDeviceOutOfDeviceMemory);
				case VkResult::VK_ERROR_INITIALIZATION_FAILED:						throw backend_error(BackendErrorInitializationFailed);
				case VkResult::VK_ERROR_DEVICE_LOST:								throw backend_error(BackendErrorDeviceLost);
				case VkResult::VK_ERROR_MEMORY_MAP_FAILED:							throw backend_error(BackendErrorMemoryMappingFailed);
				case VkResult::VK_ERROR_LAYER_NOT_PRESENT:							throw backend_error(BackendErrorVulkanLayerNotPresent);
				case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT:						throw backend_error(BackendErrorVulkanExtensionNotPresent);
				case VkResult::VK_ERROR_FEATURE_NOT_PRESENT:						throw backend_error(BackendErrorDeviceFeatureNotPresent);
				case VkResult::VK_ERROR_INCOMPATIBLE_DRIVER:						throw backend_error(BackendErrorIncompatibleDriver);
				case VkResult::VK_ERROR_TOO_MANY_OBJECTS:							throw backend_error(BackendErrorTooManyObjects);
				case VkResult::VK_ERROR_FORMAT_NOT_SUPPORTED:						throw backend_error(BackendErrorFormatNotSupported);
				case VkResult::VK_ERROR_FRAGMENTED_POOL:							throw backend_error(BackendErrorFragmentedPool);
				case VkResult::VK_ERROR_OUT_OF_POOL_MEMORY:							throw backend_error(BackendErrorOutOfPoolMemory);
				case VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE:					throw backend_error(BackendErrorInvalidHandle);
				case VkResult::VK_ERROR_FRAGMENTATION:								throw backend_error(BackendErrorInternalFragmentation);
				case VkResult::VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:				throw backend_error(BackendErrorVulkanInvalidOpaqueCaptureAddress);
				case VkResult::VK_ERROR_SURFACE_LOST_KHR:							throw backend_error(BackendErrorDisplayLost);
				case VkResult::VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:					throw backend_error(BackendErrorDisplayInUse);
				case VkResult::VK_SUBOPTIMAL_KHR:									throw backend_error(BackendErrorRenderTargetSuboptimal);
				case VkResult::VK_ERROR_OUT_OF_DATE_KHR:							throw backend_error(BackendErrorRenderTargetOutOfDate);
				case VkResult::VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:					throw backend_error(BackendErrorIncompatibleDisplay);
				case VkResult::VK_ERROR_VALIDATION_FAILED_EXT:						throw backend_error(BackendErrorValidationFailed);
				case VkResult::VK_ERROR_INVALID_SHADER_NV:							throw backend_error(BackendErrorInvalidShader);
				case VkResult::VK_ERROR_NOT_PERMITTED_EXT:							throw backend_error(BackendErrorOperationNotPermitted);
				case VkResult::VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:		throw backend_error(BackendErrorFullScreenModeLost);
				case VkResult::VK_THREAD_IDLE_KHR:									throw backend_error(BackendErrorDeviceThreadIdle);
				case VkResult::VK_THREAD_DONE_KHR:									throw backend_error(BackendErrorDeviceThreadDone);
				case VkResult::VK_OPERATION_DEFERRED_KHR:							throw backend_error(BackendErrorOperationDeferred);
				case VkResult::VK_OPERATION_NOT_DEFERRED_KHR:						throw backend_error(BackendErrorOperationNotDeferred);
				case VkResult::VK_PIPELINE_COMPILE_REQUIRED_EXT:					throw backend_error(BackendErrorPipelineCompilationRequired);
				default:															throw backend_error(BackendErrorUnknown);
				}
			}

			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage>& vImages, VkFormat imageFormat, VulkanDevice& device, VkImageAspectFlags aspectFlags, VkImageViewType viewType, uint32 layerCount, uint32 baseLayerIndex, uint32 mipLevels, uint32 baseMipLevel, VkComponentMapping mapping)
			{
				VkImageViewCreateInfo vCreateInfo = {};
				vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				vCreateInfo.flags = 0;
				vCreateInfo.pNext = VK_NULL_HANDLE;
				vCreateInfo.viewType = viewType;
				vCreateInfo.format = imageFormat;
				vCreateInfo.components = mapping;
				vCreateInfo.subresourceRange.layerCount = layerCount;
				vCreateInfo.subresourceRange.baseArrayLayer = baseLayerIndex;
				vCreateInfo.subresourceRange.levelCount = mipLevels;
				vCreateInfo.subresourceRange.baseMipLevel = baseMipLevel;
				vCreateInfo.subresourceRange.aspectMask = aspectFlags;

				std::vector<VkImageView> vImageViews(vImages.size());
				VkImageView* pArray = vImageViews.data();
				for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
				{
					vCreateInfo.image = *itr;
					FLINT_VK_ASSERT(device.GetDeviceTable().vkCreateImageView(device.GetLogicalDevice(), &vCreateInfo, nullptr, pArray));
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

				throw std::runtime_error("Unable to find suitable format!");
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
				case Flint::ShaderResourceType::Sampler:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;

				case Flint::ShaderResourceType::CombinedImageSampler:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

				case Flint::ShaderResourceType::SampledImage:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

				case Flint::ShaderResourceType::StorageImage:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

				case Flint::ShaderResourceType::UniformTexelBuffer:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;

				case Flint::ShaderResourceType::StorageTexelBuffer:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;

				case Flint::ShaderResourceType::UniformBuffer:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

				case Flint::ShaderResourceType::StorageBuffer:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

				case Flint::ShaderResourceType::UniformBufferDynamic:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

				case Flint::ShaderResourceType::StorageBufferDynamic:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

				case Flint::ShaderResourceType::InputAttachment:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

				case Flint::ShaderResourceType::AccelerationStructure:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;

				default:
					throw backend_error("Invalid or undefined shader resource type!");
				}

				return VkDescriptorType();
			}

			VkShaderStageFlags GetShaderStage(const ShaderType type)
			{
				switch (type)
				{
				case Flint::ShaderType::Vertex:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

				case Flint::ShaderType::TessellationControl:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

				case Flint::ShaderType::TessellationEvaluation:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

				case Flint::ShaderType::Geometry:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;

				case Flint::ShaderType::Fragment:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;

				case Flint::ShaderType::Compute:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;

				case Flint::ShaderType::RayGen:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_RAYGEN_BIT_KHR;

				case Flint::ShaderType::AnyHit:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

				case Flint::ShaderType::ClosestHit:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

				case Flint::ShaderType::RayMiss:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_MISS_BIT_KHR;

				default:
					throw std::runtime_error("Invalid or Undefined shader type!");
				}

				return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
			}

			PixelFormat GetPixelFormat(VkFormat format)
			{
				switch (format)
				{
				case VK_FORMAT_UNDEFINED:
					return PixelFormat::Undefined;

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
					throw backend_error("Unsupported format!");
				}

				return PixelFormat::Undefined;
			}

			VkFormat GetVulkanFormat(const PixelFormat format)
			{
				switch (format)
				{
				case Flint::PixelFormat::Undefined:
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

				case Flint::PixelFormat::S8_UINT:
					return VkFormat::VK_FORMAT_S8_UINT;

				case Flint::PixelFormat::D16_UNORMAL_S8_UINT:
					return VkFormat::VK_FORMAT_D16_UNORM_S8_UINT;

				case Flint::PixelFormat::D24_UNORMAL_S8_UINT:
					return VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;

				case Flint::PixelFormat::D32_SFLOAT_S8_UINT:
					return VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;

				default:
					throw backend_error("Invalid pixel format!");
				}

				return VkFormat::VK_FORMAT_UNDEFINED;
			}

			uint8 GetByteDepth(const PixelFormat format)
			{
				switch (format)
				{
				case Flint::PixelFormat::R8_SRGB:
				case Flint::PixelFormat::R8_UNORMAL:
				case Flint::PixelFormat::S8_UINT:
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
				case Flint::PixelFormat::D16_UNORMAL_S8_UINT:
					return 3;

				case Flint::PixelFormat::R8G8B8A8_SRGB:
				case Flint::PixelFormat::R8G8B8A8_UNORMAL:
				case Flint::PixelFormat::B8G8R8A8_SRGB:
				case Flint::PixelFormat::B8G8R8A8_UNORMAL:
				case Flint::PixelFormat::R16G16_SFLOAT:
				case Flint::PixelFormat::D32_SFLOAT:
				case Flint::PixelFormat::R32_SFLOAT:
				case Flint::PixelFormat::D24_UNORMAL_S8_UINT:
					return 4;

				case Flint::PixelFormat::D32_SFLOAT_S8_UINT:
					return 5;

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

			VkSampleCountFlagBits GetSampleCount(MultiSampleCount count)
			{
				switch (count)
				{
				case Flint::MultiSampleCount::One:
					return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

				case Flint::MultiSampleCount::Two:
					return VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;

				case Flint::MultiSampleCount::Four:
					return VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;

				case Flint::MultiSampleCount::Eight:
					return VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;

				case Flint::MultiSampleCount::Sixteen:
					return VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;

				case Flint::MultiSampleCount::ThirtyTwo:
					return VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;

				case Flint::MultiSampleCount::SixtyFour:
					return VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;

				default:
					throw backend_error("Invalid sample count!");
				}

				return VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
			}

			VkImageAspectFlags GetImageAspectFlags(const ImageUsage usage)
			{
				if ((usage & ImageUsage::Depth) == ImageUsage::Depth)
					return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;

				VkImageAspectFlags vFlags = 0;

				if ((usage & ImageUsage::Graphics) == ImageUsage::Graphics)
					vFlags |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

				if ((usage & ImageUsage::Storage) == ImageUsage::Storage)
					vFlags |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

				if ((usage & ImageUsage::Color) == ImageUsage::Color)
					vFlags |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

				return vFlags;
			}

			VkAccessFlags GetAccessFlags(VkImageLayout layout)
			{
				switch (layout)
				{
				case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_PREINITIALIZED: return  VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL: break;
				case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: break;
				case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR: break;
				case VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV: break;
				case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT: break;
				//case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR: break;
				//case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR: break;
				//case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR: break;
				//case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR: break;
				//case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR: break;
				//case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR: break;
				//case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR: break;
				}

				return 0;
			}

			VkPipelineStageFlags GetPipelineStageFlags(VkAccessFlags flags)
			{
				switch (flags)
				{
				case VK_ACCESS_INDIRECT_COMMAND_READ_BIT:						return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				case VK_ACCESS_INDEX_READ_BIT:									return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
				case VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT:						return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
				case VK_ACCESS_UNIFORM_READ_BIT:								return VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
				case VK_ACCESS_SHADER_READ_BIT:									return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
				case VK_ACCESS_SHADER_WRITE_BIT:								return VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
				case VK_ACCESS_INPUT_ATTACHMENT_READ_BIT:						return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				case VK_ACCESS_COLOR_ATTACHMENT_READ_BIT:						return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				case VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT:						return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT:				return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:				return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				case VK_ACCESS_TRANSFER_READ_BIT:								return VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				case VK_ACCESS_TRANSFER_WRITE_BIT:								return VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				case VK_ACCESS_HOST_READ_BIT:									return VK_PIPELINE_STAGE_HOST_BIT;
				case VK_ACCESS_HOST_WRITE_BIT:									return VK_PIPELINE_STAGE_HOST_BIT;
				case VK_ACCESS_MEMORY_READ_BIT:									return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
				case VK_ACCESS_MEMORY_WRITE_BIT:								return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
				case VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT:		return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				case VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV:					return VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
				case VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV:					return VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
				case VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT:				return VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
				case VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR:	return VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
					//case VK_ACCESS_2_INVOCATION_MASK_READ_BIT_HUAWEI:				return VK_PIPELINE_STAGE_2_INVOCATION_MASK_BIT_HUAWEI;
				case VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT:				return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
				case VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT:		return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
				case VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT:			return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
				case VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR:				return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				case VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR:			return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				case VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT:				return VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
				}

				return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			}

			void AddPushConstantRangesToVector(std::vector<VkPushConstantRange>& ranges, const VulkanShader& shader)
			{
				std::vector<VkPushConstantRange> tempRanges = shader.GetPushConstantRanges();
				ranges.insert(ranges.end(), tempRanges.begin(), tempRanges.end());
			}

			void AddResourcesToMap(TShaderResourceMap& resources, const VulkanShader& shader)
			{
				const TShaderResourceMap tempResources = shader.GetShaderResources();
				resources.insert(tempResources.begin(), tempResources.end());
			}
		}
	}
}