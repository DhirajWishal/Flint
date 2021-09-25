// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanImageSampler.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Helpers
		{
			VkSamplerAddressMode GetAddressMode(AddressMode mode)
			{
				switch (mode)
				{
				case Flint::AddressMode::Repeat:
					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;

				case Flint::AddressMode::MirroredRepeat:
					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

				case Flint::AddressMode::ClampToEdge:
					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

				case Flint::AddressMode::ClampToBorder:
					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

				case Flint::AddressMode::MirrorClampToEdge:
					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;

				default:
					throw backend_error("Invalid address mode!");
				}

				return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
			}

			VkBorderColor GetBorderColor(BorderColor color)
			{
				switch (color)
				{
				case Flint::BorderColor::TransparentBlackFLOAT:
					return VkBorderColor::VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

				case Flint::BorderColor::TransparentBlackINT:
					return VkBorderColor::VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;

				case Flint::BorderColor::OpaqueBlackFLOAT:
					return VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

				case Flint::BorderColor::OpaqueBlackINT:
					return VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;

				case Flint::BorderColor::OpaqueWhiteFLOAT:
					return VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

				case Flint::BorderColor::OpaqueWhiteINT:
					return VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_WHITE;

				default:
					throw backend_error("Invalid border color!");
				}

				return VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			}

			VkCompareOp GetCompareOperator(CompareOperator op)
			{
				switch (op)
				{
				case Flint::CompareOperator::Never:
					return VkCompareOp::VK_COMPARE_OP_NEVER;

				case Flint::CompareOperator::Less:
					return VkCompareOp::VK_COMPARE_OP_LESS;

				case Flint::CompareOperator::Equal:
					return VkCompareOp::VK_COMPARE_OP_EQUAL;

				case Flint::CompareOperator::LessOrEqual:
					return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;

				case Flint::CompareOperator::Greater:
					return VkCompareOp::VK_COMPARE_OP_GREATER;

				case Flint::CompareOperator::NotEqual:
					return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;

				case Flint::CompareOperator::GreaterOrEqual:
					return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;

				case Flint::CompareOperator::Always:
					return VkCompareOp::VK_COMPARE_OP_ALWAYS;

				default:
					throw backend_error("Invalid compare operator!");
				}

				return VkCompareOp::VK_COMPARE_OP_ALWAYS;
			}

			VkFilter GetFilter(ImageFilter filter)
			{
				switch (filter)
				{
				case Flint::ImageFilter::Nearest:
					return VkFilter::VK_FILTER_NEAREST;

				case Flint::ImageFilter::Linear:
					return VkFilter::VK_FILTER_LINEAR;

				case Flint::ImageFilter::CubicImage:
					return VkFilter::VK_FILTER_CUBIC_IMG;

				default:
					throw backend_error("Invalid image filter!");
				}

				return VkFilter::VK_FILTER_LINEAR;
			}

			VkSamplerMipmapMode GetMipMapMode(ImageMipMapMode mode)
			{
				switch (mode)
				{
				case Flint::ImageMipMapMode::Nearest:
					return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;

				case Flint::ImageMipMapMode::Linear:
					return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;

				default:
					throw backend_error("Invalid image mip map filter!");
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
			vCreateInfo.addressModeU = Helpers::GetAddressMode(specification.mAddressModeU);
			vCreateInfo.addressModeV = Helpers::GetAddressMode(specification.mAddressModeV);
			vCreateInfo.addressModeW = Helpers::GetAddressMode(specification.mAddressModeW);
			vCreateInfo.anisotropyEnable = GET_VK_BOOL(specification.bEnableAnisotropy);
			vCreateInfo.borderColor = Helpers::GetBorderColor(specification.mBorderColor);
			vCreateInfo.compareEnable = GET_VK_BOOL(specification.bEnableCompare);
			vCreateInfo.compareOp = Helpers::GetCompareOperator(specification.mCompareOperator);
			vCreateInfo.magFilter = Helpers::GetFilter(specification.mImageMagificationFilter);
			vCreateInfo.minFilter = Helpers::GetFilter(specification.mImageMinificationFilter);
			vCreateInfo.maxAnisotropy = specification.mMaxAnisotrophy;
			vCreateInfo.maxLod = specification.mMaxLevelOfDetail;
			vCreateInfo.minLod = specification.mMinLevelOfDetail;
			vCreateInfo.mipLodBias = specification.mMipLODBias;
			vCreateInfo.mipmapMode = Helpers::GetMipMapMode(specification.mMipMapMode);
			vCreateInfo.unnormalizedCoordinates = GET_VK_BOOL(specification.bEnableUnnormalizedCoordinates);

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			if (vCreateInfo.maxAnisotropy == 0.0f)
			{
				VkPhysicalDeviceProperties vProperties = {};
				vkGetPhysicalDeviceProperties(vDevice.GetPhysicalDevice(), &vProperties);
				vCreateInfo.maxAnisotropy = vProperties.limits.maxSamplerAnisotropy;
			}

			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateSampler(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vSampler));
		}

		void VulkanImageSampler::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkDestroySampler(vDevice.GetLogicalDevice(), vSampler, nullptr);

			bIsTerminated = true;
		}
	}
}