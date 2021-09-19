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
				case VK_NOT_READY:										throw backend_error(BackendErrorDeviceNotReady);
				case VK_TIMEOUT:										throw backend_error(BackendErrorDeviceTimeout);
				case VK_EVENT_SET:										throw backend_error(BackendErrorEventSet);
				case VK_EVENT_RESET:									throw backend_error(BackendErrorEventReset);
				case VK_INCOMPLETE:										throw backend_error(BackendErrorIncompleteCommand);
				case VK_ERROR_OUT_OF_HOST_MEMORY:						throw backend_error(BackendErrorDeviceOutOfHostMemory);
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:						throw backend_error(BackendErrorDeviceOutOfDeviceMemory);
				case VK_ERROR_INITIALIZATION_FAILED:					throw backend_error(BackendErrorInitializationFailed);
				case VK_ERROR_DEVICE_LOST:								throw backend_error(BackendErrorDeviceLost);
				case VK_ERROR_MEMORY_MAP_FAILED:						throw backend_error(BackendErrorMemoryMappingFailed);
				case VK_ERROR_LAYER_NOT_PRESENT:						throw backend_error(BackendErrorVulkanLayerNotPresent);
				case VK_ERROR_EXTENSION_NOT_PRESENT:					throw backend_error(BackendErrorVulkanExtensionNotPresent);
				case VK_ERROR_FEATURE_NOT_PRESENT:						throw backend_error(BackendErrorDeviceFeatureNotPresent);
				case VK_ERROR_INCOMPATIBLE_DRIVER:						throw backend_error(BackendErrorIncompatibleDriver);
				case VK_ERROR_TOO_MANY_OBJECTS:							throw backend_error(BackendErrorTooManyObjects);
				case VK_ERROR_FORMAT_NOT_SUPPORTED:						throw backend_error(BackendErrorFormatNotSupported);
				case VK_ERROR_FRAGMENTED_POOL:							throw backend_error(BackendErrorFragmentedPool);
				case VK_ERROR_OUT_OF_POOL_MEMORY:						throw backend_error(BackendErrorOutOfPoolMemory);
				case VK_ERROR_INVALID_EXTERNAL_HANDLE:					throw backend_error(BackendErrorInvalidHandle);
				case VK_ERROR_FRAGMENTATION:							throw backend_error(BackendErrorInternalFragmentation);
				case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:			throw backend_error(BackendErrorVulkanInvalidOpaqueCaptureAddress);
				case VK_ERROR_SURFACE_LOST_KHR:							throw backend_error(BackendErrorDisplayLost);
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:					throw backend_error(BackendErrorDisplayInUse);
				case VK_SUBOPTIMAL_KHR:									throw backend_error(BackendErrorRenderTargetSuboptimal);
				case VK_ERROR_OUT_OF_DATE_KHR:							throw backend_error(BackendErrorRenderTrargetOutOfDate);
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:					throw backend_error(BackendErrorIncompatibleDisplay);
				case VK_ERROR_VALIDATION_FAILED_EXT:					throw backend_error(BackendErrorValidationFailed);
				case VK_ERROR_INVALID_SHADER_NV:						throw backend_error(BackendErrorInvalidShader);
				case VK_ERROR_NOT_PERMITTED_EXT:						throw backend_error(BackendErrorOperationNotPermitted);
				case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:		throw backend_error(BackendErrorFullScreenModeLost);
				case VK_THREAD_IDLE_KHR:								throw backend_error(BackendErrorDeviceThreadIdle);
				case VK_THREAD_DONE_KHR:								throw backend_error(BackendErrorDeviceThreadDone);
				case VK_OPERATION_DEFERRED_KHR:							throw backend_error(BackendErrorOperationDiferred);
				case VK_OPERATION_NOT_DEFERRED_KHR:						throw backend_error(BackendErrorOperationNotDeferred);
				case VK_PIPELINE_COMPILE_REQUIRED_EXT:					throw backend_error(BackendErrorPipelineCompilationRequired);
				default:												throw backend_error(BackendErrorUnknown);
				}
			}

			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage>& vImages, VkFormat imageFormat, VulkanDevice& device, VkImageAspectFlags aspectFlags, VkImageViewType viewType, UI32 layerCount, UI32 baseLayerIndex, UI32 mipLevels, UI32 baseMipLevel, VkComponentMapping mapping)
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
				vSR.levelCount = mipLevels;
				vSR.baseMipLevel = baseMipLevel;
				vSR.aspectMask = aspectFlags;

				vCreateInfo.subresourceRange = vSR;

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
					FLINT_THROW_RUNTIME_ERROR("Invalid or Undefined shader type!");
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

			UI8 GetByteDepth(const PixelFormat format)
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