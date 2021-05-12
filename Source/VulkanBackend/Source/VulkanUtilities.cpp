// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanUtilities.h"
#include "VulkanBackend/VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Utilities
		{
			VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
			{
				for (const auto& availableFormat : availableFormats)
					if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
						&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
						return availableFormat;

				return availableFormats[0];
			}

			VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
			{
				VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

				for (const auto& availablePresentMode : availablePresentModes)
				{
					if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
						return availablePresentMode;
					else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
						bestMode = availablePresentMode;
				}

				return bestMode;
			}

			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, UI32 width, UI32 height)
			{
				VkExtent2D actualExtent = {
					width,
					height
				};

				if ((width >= capabilities.maxImageExtent.width) || (width <= capabilities.minImageExtent.width))
					actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));

				if ((height >= capabilities.maxImageExtent.height) || (height <= capabilities.minImageExtent.height))
					actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

				return actualExtent;
			}

			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage>& vImages, VkFormat imageFormat, VulkanDevice* pDevice, VkImageAspectFlags aspectFlags)
			{
				VkImageViewCreateInfo vCI = {};
				vCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				vCI.flags = VK_NULL_HANDLE;
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
					FLINT_VK_ASSERT(pDevice->CreateImageView(&vCI, pArray), "Failed to create image views!");
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

				FLINT_LOG_FATAL(TEXT("failed to find supported format!"));
				return VkFormat();
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

			bool HasStencilComponent(VkFormat vFormat)
			{
				return vFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || vFormat == VK_FORMAT_D24_UNORM_S8_UINT;
			}

			VkShaderStageFlagBits GetShaderStage(ShaderLocation location)
			{
				switch (location)
				{
				case Flint::ShaderLocation::VERTEX:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

				case Flint::ShaderLocation::TESSELLATION:
					return VkShaderStageFlagBits(VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);

				case Flint::ShaderLocation::GEOMETRY:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;

				case Flint::ShaderLocation::FRAGMENT:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
					break;

				case Flint::ShaderLocation::COMPUTE:
					return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined shader location!"))
						break;
				}

				return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
			}

			VkDescriptorType GetDescriptorType(UniformType type)
			{
				switch (type)
				{
				case Flint::UniformType::UNIFORM_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

				case Flint::UniformType::STORAGE_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

				case Flint::UniformType::UNIFORM_BUFFER_DYNAMIC:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

				case Flint::UniformType::STORAGE_BUFFER_DYNAMIC:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

				case Flint::UniformType::UNIFORM_TEXEL_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;

				case Flint::UniformType::STORAGE_TEXEL_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;

				case Flint::UniformType::INPUT_ATTACHMENT:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

				case Flint::UniformType::STORAGE_IMAGE:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

				case Flint::UniformType::SAMPLER_2D:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

				case Flint::UniformType::SAMPLER_CUBE:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

				case Flint::UniformType::SAMPLER_2D_ARRAY:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;

				case Flint::UniformType::SAMPLER_CUBE_ARRAY:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;

				case Flint::UniformType::ACCELERATION_STRUCTURE:
					return VkDescriptorType(VkDescriptorType::VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR | VkDescriptorType::VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV);

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined uniform type!"))
						break;
				}

				return VkDescriptorType();
			}

			VkFormat GetFormatFromSize(UI64 size)
			{
				switch (size)
				{
					case sizeof(UI8) :
						return VkFormat::VK_FORMAT_R8_SINT;

					case sizeof(UI8) * 2:
						return VkFormat::VK_FORMAT_R8G8_SINT;

						case sizeof(UI32) :
							return VkFormat::VK_FORMAT_R32_SFLOAT;

						case sizeof(UI32) * 2:
							return VkFormat::VK_FORMAT_R32G32_SFLOAT;

						case sizeof(UI32) * 4:
							return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;

						default:
							FLINT_LOG_ERROR(TEXT("Invalid or Unsupported size!"))
								break;
				}

				return VkFormat::VK_FORMAT_UNDEFINED;
			}

			std::vector<VkVertexInputAttributeDescription> GetInputAttributeDescriptions(const FShaderDigest& digest)
			{
				std::vector<VkVertexInputAttributeDescription> vDescriptions = {};
				VkVertexInputAttributeDescription vDesc = {};
				vDesc.binding = 0;
				vDesc.offset = 0;

				for (auto itr = digest.mInputAttributes.begin(); itr != digest.mInputAttributes.end(); itr++)
				{
					vDesc.location = itr->mLocation;
					vDesc.format = GetFormatFromSize(itr->mSize);

					INSERT_INTO_VECTOR(vDescriptions, vDesc);
					vDesc.offset += static_cast<UI32>(itr->mSize);
				}

				return vDescriptions;
			}

			UI64 GetStride(const FShaderDigest& digest)
			{
				UI64 size = 0;
				for (auto itr = digest.mInputAttributes.begin(); itr != digest.mInputAttributes.end(); itr++)
					size += itr->mSize;

				return size;
			}
		}
	}
}