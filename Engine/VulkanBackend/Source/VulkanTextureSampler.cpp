// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanTextureSampler.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace /* anonymous */
{
	/**
	 * Get the address mode.
	 *
	 * @param mode The flint address mode.
	 * @return The Vulkan address mode.
	 */
	VkSamplerAddressMode GetAddressMode(Flint::AddressMode mode)
	{
		switch (mode)
		{
		case Flint::AddressMode::Repeat:					return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case Flint::AddressMode::MirroredRepeat:			return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case Flint::AddressMode::ClampToEdge:				return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case Flint::AddressMode::ClampToBorder:				return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case Flint::AddressMode::MirrorClampToEdge:			return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		default:											throw Flint::BackendError("Invalid address mode!");
		}
	}

	/**
	 * Get the border color.
	 *
	 * @param color The flint border color.
	 * @return The Vulkan border color.
	 */
	VkBorderColor GetBorderColor(Flint::BorderColor color)
	{
		switch (color)
		{
		case Flint::BorderColor::TransparentBlackFLOAT:		return VkBorderColor::VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		case Flint::BorderColor::TransparentBlackINT:		return VkBorderColor::VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
		case Flint::BorderColor::OpaqueBlackFLOAT:			return VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		case Flint::BorderColor::OpaqueBlackINT:			return VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		case Flint::BorderColor::OpaqueWhiteFLOAT:			return VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		case Flint::BorderColor::OpaqueWhiteINT:			return VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_WHITE;
		default:											throw Flint::BackendError("Invalid border color!");
		}
	}

	/**
	 * Get the compare operator.
	 *
	 * @param op The flint operator.
	 * @return The Vulkan compare operator.
	 */
	VkCompareOp GetCompareOperator(Flint::CompareOperator op)
	{
		switch (op)
		{
		case Flint::CompareOperator::Never:					return VkCompareOp::VK_COMPARE_OP_NEVER;
		case Flint::CompareOperator::Less:					return VkCompareOp::VK_COMPARE_OP_LESS;
		case Flint::CompareOperator::Equal:					return VkCompareOp::VK_COMPARE_OP_EQUAL;
		case Flint::CompareOperator::LessOrEqual:			return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
		case Flint::CompareOperator::Greater:				return VkCompareOp::VK_COMPARE_OP_GREATER;
		case Flint::CompareOperator::NotEqual:				return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;
		case Flint::CompareOperator::GreaterOrEqual:		return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;
		case Flint::CompareOperator::Always:				return VkCompareOp::VK_COMPARE_OP_ALWAYS;
		default:											throw Flint::BackendError("Invalid compare operator!");
		}
	}

	/**
	 * Get the filter type.
	 *
	 * @param filter The flint filter.
	 * @return The VUlkan filter.
	 */
	VkFilter GetFilter(Flint::ImageFilter filter)
	{
		switch (filter)
		{
		case Flint::ImageFilter::Nearest:					return VkFilter::VK_FILTER_NEAREST;
		case Flint::ImageFilter::Linear:					return VkFilter::VK_FILTER_LINEAR;
		case Flint::ImageFilter::CubicImage:				return VkFilter::VK_FILTER_CUBIC_IMG;
		default:											throw Flint::BackendError("Invalid image filter!");
		}
	}

	/**
	 * Get the mipmap mode.
	 *
	 * @param mode The flint mode.
	 * @return The Vulkan mode.
	 */
	VkSamplerMipmapMode GetMipMapMode(Flint::ImageMipMapMode mode)
	{
		switch (mode)
		{
		case Flint::ImageMipMapMode::Nearest:				return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case Flint::ImageMipMapMode::Linear:				return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
		default:											throw Flint::BackendError("Invalid image mip map filter!");
		}
	}
}

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanTextureSampler::VulkanTextureSampler(const std::shared_ptr<VulkanDevice>& pDevice, TextureSamplerSpecification&& specification)
			: TextureSampler(pDevice, std::move(specification))
		{
			VkSamplerCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;
			createInfo.addressModeU = GetAddressMode(m_Specification.mAddressModeU);
			createInfo.addressModeV = GetAddressMode(m_Specification.mAddressModeV);
			createInfo.addressModeW = GetAddressMode(m_Specification.mAddressModeW);
			createInfo.anisotropyEnable = GET_VK_BOOL(m_Specification.bEnableAnisotropy);
			createInfo.maxAnisotropy = m_Specification.mMaxAnisotrophy;
			createInfo.borderColor = GetBorderColor(m_Specification.mBorderColor);
			createInfo.compareEnable = GET_VK_BOOL(m_Specification.bEnableCompare);
			createInfo.compareOp = GetCompareOperator(m_Specification.mCompareOperator);
			createInfo.magFilter = GetFilter(m_Specification.mImageMagificationFilter);
			createInfo.minFilter = GetFilter(m_Specification.mImageMinificationFilter);
			createInfo.maxLod = m_Specification.mMaxLevelOfDetail;
			createInfo.minLod = m_Specification.mMinLevelOfDetail;
			createInfo.mipLodBias = m_Specification.mMipLODBias;
			createInfo.mipmapMode = GetMipMapMode(m_Specification.mMipMapMode);
			createInfo.unnormalizedCoordinates = GET_VK_BOOL(m_Specification.bEnableUnnormalizedCoordinates);

			if (createInfo.maxAnisotropy == 0.0f && m_Specification.bEnableAnisotropy)
			{
				VkPhysicalDeviceProperties vProperties = {};
				vkGetPhysicalDeviceProperties(pDevice->getPhysicalDevice(), &vProperties);
				createInfo.maxAnisotropy = vProperties.limits.maxSamplerAnisotropy;
			}

			FLINT_VK_ASSERT(pDevice->getDeviceTable().vkCreateSampler(pDevice->getLogicalDevice(), &createInfo, nullptr, &m_Sampler), "Failed to create the sampler!");

			validate();
		}

		VulkanTextureSampler::~VulkanTextureSampler()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanTextureSampler::terminate()
		{
			getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroySampler(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_Sampler, nullptr);
			invalidate();
		}
	}
}