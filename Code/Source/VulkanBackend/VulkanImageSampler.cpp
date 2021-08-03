// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanImageSampler.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			VkSamplerAddressMode GetAddressMode(AddressMode mode)
			{
				switch (mode)
				{
				case Flint::AddressMode::REPEAT:
					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;

				case Flint::AddressMode::MIRRORED_REPEAT:
					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

				case Flint::AddressMode::CLAMP_TO_EDGE:
					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

				case Flint::AddressMode::CLAMP_TO_BORDER:
					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

				case Flint::AddressMode::MIRROR_CLAMP_TO_EDGE:
					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid address mode!");
				}

				return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
			}

			VkBorderColor GetBorderColor(BorderColor color)
			{
				switch (color)
				{
				case Flint::BorderColor::FLOAT_TRANSPARENT_BLACK:
					return VkBorderColor::VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

				case Flint::BorderColor::INT_TRANSPARENT_BLACK:
					return VkBorderColor::VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;

				case Flint::BorderColor::FLOAT_OPAQUE_BLACK:
					return VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

				case Flint::BorderColor::INT_OPAQUE_BLACK:
					return VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;

				case Flint::BorderColor::FLOAT_OPAQUE_WHITE:
					return VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

				case Flint::BorderColor::INT_OPAQUE_WHITE:
					return VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_WHITE;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid border color!");
				}

				return VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			}

			VkCompareOp GetCompareOperator(CompareOperator op)
			{
				switch (op)
				{
				case Flint::CompareOperator::NEVER:
					return VkCompareOp::VK_COMPARE_OP_NEVER;

				case Flint::CompareOperator::LESS:
					return VkCompareOp::VK_COMPARE_OP_LESS;

				case Flint::CompareOperator::EQUAL:
					return VkCompareOp::VK_COMPARE_OP_EQUAL;

				case Flint::CompareOperator::LESS_OR_EQUAL:
					return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;

				case Flint::CompareOperator::GREATER:
					return VkCompareOp::VK_COMPARE_OP_GREATER;

				case Flint::CompareOperator::NOT_EQUAL:
					return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;

				case Flint::CompareOperator::GREATER_OR_EQUAL:
					return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;

				case Flint::CompareOperator::ALWAYS:
					return VkCompareOp::VK_COMPARE_OP_ALWAYS;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid compare operator!");
				}

				return VkCompareOp::VK_COMPARE_OP_ALWAYS;
			}

			VkFilter GetFilter(ImageFilter filter)
			{
				switch (filter)
				{
				case Flint::ImageFilter::NEAREST:
					return VkFilter::VK_FILTER_NEAREST;

				case Flint::ImageFilter::LINEAR:
					return VkFilter::VK_FILTER_LINEAR;

				case Flint::ImageFilter::CUBIC_IMG:
					return VkFilter::VK_FILTER_CUBIC_IMG;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid image filter!");
				}

				return VkFilter::VK_FILTER_LINEAR;
			}

			VkSamplerMipmapMode GetMipMapMode(ImageMipMapMode mode)
			{
				switch (mode)
				{
				case Flint::ImageMipMapMode::NEAREST:
					return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;

				case Flint::ImageMipMapMode::LINEAR:
					return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid image mip map filter!");
				}

				return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
			}
		}

		VulkanImageSampler::VulkanImageSampler(const std::shared_ptr<Device>& pDevice, const ImageSamplerSpecification& specification)
			: ImageSampler(pDevice, specification)
		{
			VkSamplerCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.addressModeU = _Helpers::GetAddressMode(specification.mAddressModeU);
			vCreateInfo.addressModeV = _Helpers::GetAddressMode(specification.mAddressModeV);
			vCreateInfo.addressModeW = _Helpers::GetAddressMode(specification.mAddressModeW);
			vCreateInfo.anisotropyEnable = GET_VK_BOOL(specification.bEnableAnisotropy);
			vCreateInfo.borderColor = _Helpers::GetBorderColor(specification.mBorderColor);
			vCreateInfo.compareEnable = GET_VK_BOOL(specification.bEnableCompare);
			vCreateInfo.compareOp = _Helpers::GetCompareOperator(specification.mCompareOperator);
			vCreateInfo.magFilter = _Helpers::GetFilter(specification.mImageMagificationFilter);
			vCreateInfo.minFilter = _Helpers::GetFilter(specification.mImageMinificationFilter);
			vCreateInfo.maxAnisotropy = specification.mMaxAnisotrophy;
			vCreateInfo.maxLod = specification.mMaxLevelOfDetail;
			vCreateInfo.minLod = specification.mMinLevelOfDetail;
			vCreateInfo.mipLodBias = specification.mMipLoadBias;
			vCreateInfo.mipmapMode = _Helpers::GetMipMapMode(specification.mMipMapMode);
			vCreateInfo.unnormalizedCoordinates = GET_VK_BOOL(specification.bEnableUnnormalizedCoordinates);

			if (vCreateInfo.maxAnisotropy == 0.0f)
			{
				VkPhysicalDeviceProperties vProperties = {};
				vkGetPhysicalDeviceProperties(pDevice->StaticCast<VulkanDevice>().GetPhysicalDevice(), &vProperties);
				vCreateInfo.maxAnisotropy = vProperties.limits.maxSamplerAnisotropy;
			}

			FLINT_VK_ASSERT(vkCreateSampler(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCreateInfo, nullptr, &vSampler));
		}

		void VulkanImageSampler::Terminate()
		{
			vkDestroySampler(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vSampler, nullptr);
		}
	}
}