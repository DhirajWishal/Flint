// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend\VulkanImage.h"
#include "VulkanBackend\VulkanMacros.h"
#include "VulkanBackend\VulkanOneTimeCommandBuffer.h"
#include "VulkanBackend\VulkanBuffer.h"
#include "VulkanBackend\VulkanUtilities.h"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			VkImageType GetType(Backend::ImageUsage usage)
			{
				if (usage == Backend::ImageUsage::GRAPHICS_1D || usage == Backend::ImageUsage::STORAGE_1D)
					return VkImageType::VK_IMAGE_TYPE_1D;

				if (usage == Backend::ImageUsage::GRAPHICS_3D || usage == Backend::ImageUsage::STORAGE_3D)
					return VkImageType::VK_IMAGE_TYPE_3D;

				return VkImageType::VK_IMAGE_TYPE_2D;
			}

			VkImageUsageFlags GetUsage(Backend::ImageUsage usage)
			{
				if (usage == Backend::ImageUsage::GRAPHICS_1D || usage == Backend::ImageUsage::GRAPHICS_2D || usage == Backend::ImageUsage::GRAPHICS_3D || usage == Backend::ImageUsage::GRAPHICS_CUBEMAP)
					return VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;

				return VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;
			}

			VkFormat GetFormat(UI8 bitsPerChannel)
			{
				UI8 channels = bitsPerChannel / 8;
				bitsPerChannel = 8;

				if (channels == 1)
				{
					if (bitsPerChannel == 8)
						return VkFormat::VK_FORMAT_R8_UINT;

					if (bitsPerChannel == 16)
						return VkFormat::VK_FORMAT_R16_SFLOAT;

					if (bitsPerChannel == 32)
						return VkFormat::VK_FORMAT_R32_SFLOAT;

					if (bitsPerChannel == 64)
						return VkFormat::VK_FORMAT_R64_SFLOAT;
				}

				if (channels == 2)
				{
					if (bitsPerChannel == 8)
						return VkFormat::VK_FORMAT_R8G8_UINT;

					if (bitsPerChannel == 16)
						return VkFormat::VK_FORMAT_R16G16_SFLOAT;

					if (bitsPerChannel == 32)
						return VkFormat::VK_FORMAT_R32G32_SFLOAT;

					if (bitsPerChannel == 64)
						return VkFormat::VK_FORMAT_R64G64_SFLOAT;
				}

				if (channels == 3)
				{
					if (bitsPerChannel == 8)
						return VkFormat::VK_FORMAT_R8G8B8A8_SRGB;

					if (bitsPerChannel == 16)
						return VkFormat::VK_FORMAT_R16G16B16_SFLOAT;

					if (bitsPerChannel == 32)
						return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;

					if (bitsPerChannel == 64)
						return VkFormat::VK_FORMAT_R64G64B64_SFLOAT;
				}

				if (channels == 3)
				{
					if (bitsPerChannel == 8)
						return VkFormat::VK_FORMAT_R8G8B8A8_UINT;

					if (bitsPerChannel == 16)
						return VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT;

					if (bitsPerChannel == 32)
						return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;

					if (bitsPerChannel == 64)
						return VkFormat::VK_FORMAT_R64G64B64A64_SFLOAT;
				}

				return VkFormat::VK_FORMAT_UNDEFINED;
			}

			VkImageViewType GetImageViewType(Backend::ImageUsage usage, UI8 layers)
			{
				if (usage == Backend::ImageUsage::GRAPHICS_1D || usage == Backend::ImageUsage::STORAGE_1D)
					return layers > 1 ? VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY : VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;

				if (usage == Backend::ImageUsage::GRAPHICS_2D || usage == Backend::ImageUsage::STORAGE_2D)
					return layers > 1 ? VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY : VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

				if (usage == Backend::ImageUsage::GRAPHICS_3D || usage == Backend::ImageUsage::STORAGE_3D)
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_3D;

				if (usage == Backend::ImageUsage::GRAPHICS_CUBEMAP || usage == Backend::ImageUsage::STORAGE_CUBEMAP)
					return layers > 1 ? VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;

				return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
			}

			VkFilter GetFilter(Backend::SamplerFilter filter)
			{
				switch (filter)
				{
				case Flint::Backend::SamplerFilter::NEAREST:
					return VkFilter::VK_FILTER_NEAREST;

				case Flint::Backend::SamplerFilter::LINEAR:
					return VkFilter::VK_FILTER_LINEAR;

				case Flint::Backend::SamplerFilter::CUBIC_IMAGE:
					return VkFilter::VK_FILTER_CUBIC_IMG;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid sampler filter type!"))
						break;
				}

				return VkFilter::VK_FILTER_LINEAR;
			}
		}

		VulkanImage::VulkanImage(Backend::Device* pDevice, UI64 width, UI64 height, UI64 depth, Backend::ImageUsage usage, UI8 bitsPerPixel, UI8 layers)
			: Backend::Image(pDevice, width, height, depth, usage, bitsPerPixel, layers)
		{
			VkImageCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.flags = VK_NULL_HANDLE;
			vCI.format = vFormat;
			vCI.initialLayout = vCurrentLayout;
			vCI.usage = _Helpers::GetUsage(mUsage);
			vCI.imageType = _Helpers::GetType(mUsage);
			vCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			vCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCI.arrayLayers = static_cast<UI32>(mLayers);
			vCI.mipLevels = mMipLevel;
			vCI.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
			//vCI.samples = static_cast<VkSampleCountFlagBits>(pDevice->GetSampleCount());
			vCI.extent.width = static_cast<UI32>(width);
			vCI.extent.height = static_cast<UI32>(height);
			vCI.extent.depth = static_cast<UI32>(depth);

			if (mUsage == Backend::ImageUsage::GRAPHICS_CUBEMAP || mUsage == Backend::ImageUsage::STORAGE_CUBEMAP)
			{
				vCI.flags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
				vCI.arrayLayers = 6;
			}

			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->CreateImage(&vCI, &vImage), "Failed to create image!");
			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->CreateImageMemory({ vImage }, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vImageMemory), "Failed to allocate image memory!");

			VkImageViewCreateInfo vIVCI = {};
			vIVCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			vIVCI.pNext = VK_NULL_HANDLE;
			vIVCI.flags = VK_NULL_HANDLE;
			vIVCI.components = {};
			vIVCI.format = vCI.format;
			vIVCI.image = vImage;
			vIVCI.viewType = _Helpers::GetImageViewType(mUsage, mLayers);
			vIVCI.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			vIVCI.subresourceRange.baseArrayLayer = 0;
			vIVCI.subresourceRange.baseMipLevel = 0;
			vIVCI.subresourceRange.levelCount = mMipLevel;
			vIVCI.subresourceRange.layerCount = mLayers;

			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->CreateImageView(&vIVCI, &vImageView), "Failed to create image view!");
		}

		void VulkanImage::Terminate()
		{
			pDevice->GetAs<VulkanDevice>()->DestroyImage(vImage);
			pDevice->GetAs<VulkanDevice>()->DestroyImageView(vImageView);
			pDevice->GetAs<VulkanDevice>()->FreeMemory(vImageMemory);
		}

		void VulkanImage::CopyData(unsigned char* pData, UI64 width, UI64 widthOffset, UI64 height, UI64 heightOffset, UI64 depth, UI64 depthOffset, UI8 bitsPerPixel)
		{
			pDevice->GetAs<VulkanDevice>()->SetImageLayout(vImage, vCurrentLayout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vFormat, mLayers, 0, mMipLevel);
			UI64 size = width * height * depth * (bitsPerPixel / 8);

			VkMemoryRequirements vMR = {};
			vkGetImageMemoryRequirements(pDevice->GetAs<VulkanDevice>()->GetLogicalDevice(), vImage, &vMR);

			VulkanBuffer staggingBuffer = { pDevice, vMR.size, Backend::BufferUsage::STAGGING, Backend::MemoryProfile::TRANSFER_FRIENDLY };
			BYTE* pDataStore = static_cast<BYTE*>(staggingBuffer.MapMemory(vMR.size, 0));
			std::copy(pData, pData + size, pDataStore);

			staggingBuffer.FlushMemoryMappings();
			staggingBuffer.UnmapMemory();

			VkBufferImageCopy vBIC = {};
			vBIC.bufferOffset = 0;
			vBIC.bufferImageHeight = 0;
			vBIC.bufferRowLength = 0;
			vBIC.imageOffset.x = static_cast<UI32>(widthOffset);
			vBIC.imageOffset.y = static_cast<UI32>(heightOffset);
			vBIC.imageOffset.z = static_cast<UI32>(depthOffset);
			vBIC.imageExtent.width = static_cast<UI32>(width);
			vBIC.imageExtent.height = static_cast<UI32>(height);
			vBIC.imageExtent.depth = static_cast<UI32>(depth);
			vBIC.imageSubresource.baseArrayLayer = 0;
			vBIC.imageSubresource.mipLevel = 0;
			vBIC.imageSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			vBIC.imageSubresource.layerCount = mLayers;

			VulkanOneTimeCommandBuffer vCommandBuffer(pDevice->GetAs<VulkanDevice>());
			vkCmdCopyBufferToImage(vCommandBuffer, staggingBuffer.vBuffer, vImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vBIC);
		}

		VkSampler VulkanImage::CreateSampler(Backend::SamplerSpecification spec) const
		{
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(pDevice->GetAs<VulkanDevice>()->GetPhysicalDevice(), &properties);

			VkSamplerCreateInfo vSCI = {};
			vSCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			vSCI.pNext = VK_NULL_HANDLE;
			vSCI.flags = VK_NULL_HANDLE;
			vSCI.magFilter = _Helpers::GetFilter(spec.mFilter);
			vSCI.minFilter = vSCI.magFilter;
			vSCI.addressModeU = static_cast<VkSamplerAddressMode>(spec.mAddressModeU);
			vSCI.addressModeV = static_cast<VkSamplerAddressMode>(spec.mAddressModeV);
			vSCI.addressModeW = static_cast<VkSamplerAddressMode>(spec.mAddressModeW);
			vSCI.anisotropyEnable = VK_TRUE;
			vSCI.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
			vSCI.borderColor = static_cast<VkBorderColor>(spec.mBorderColor);
			vSCI.unnormalizedCoordinates = VK_FALSE;
			vSCI.compareEnable = VK_FALSE;
			vSCI.compareOp = VK_COMPARE_OP_ALWAYS;
			vSCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			vSCI.minLod = spec.mMinLOD;
			vSCI.maxLod = spec.mMipLevel;
			vSCI.mipLodBias = spec.mLODBias;

			VkSampler vSampler = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(pDevice->GetAs<VulkanDevice>()->CreateSampler(&vSCI, &vSampler), "Failed to create sampler!");

			return vSampler;
		}

		void VulkanImage::GenerateMipMaps()
		{
			VkFormatProperties vProps = pDevice->GetAs<VulkanDevice>()->GetFormatProperties(vFormat);

			FLINT_ASSERT(!(vProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT), TEXT("Texture format does not support linear bitting!"))

				VulkanOneTimeCommandBuffer vCommandBuffer(pDevice->GetAs<VulkanDevice>());

			for (UI32 i = 0; i < mLayers; i++)
			{
				VkImageMemoryBarrier barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.image = vImage;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.layerCount = mLayers;
				barrier.subresourceRange.levelCount = 1;
				barrier.subresourceRange.baseArrayLayer = i;

				I32 mipWidth = static_cast<I32>(mWidth);
				I32 mipHeight = static_cast<I32>(mHeight);
				I32 mipDepth = static_cast<I32>(mDepth);

				for (UI32 j = 1; j < mMipLevel; j++)
				{
					barrier.subresourceRange.baseMipLevel = j - 1;
					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

					vkCmdPipelineBarrier(vCommandBuffer,
						VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
						0, nullptr,
						0, nullptr,
						1, &barrier);

					VkImageBlit blit{};
					blit.srcOffsets[0] = { 0, 0, 0 };
					blit.srcOffsets[1] = { mipWidth, mipHeight, mipDepth };
					blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.srcSubresource.mipLevel = j - 1;
					blit.srcSubresource.baseArrayLayer = i;
					blit.srcSubresource.layerCount = mLayers;
					blit.dstOffsets[0] = { 0, 0, 0 };
					blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, mipDepth > 1 ? mipDepth / 2 : 1 };
					blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.dstSubresource.mipLevel = j;
					blit.dstSubresource.baseArrayLayer = i;
					blit.dstSubresource.layerCount = mLayers;

					vkCmdBlitImage(vCommandBuffer,
						vImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1, &blit,
						VK_FILTER_LINEAR);

					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

					vkCmdPipelineBarrier(vCommandBuffer,
						VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
						0, nullptr,
						0, nullptr,
						1, &barrier);

					if (mipWidth > 1) mipWidth /= 2;
					if (mipHeight > 1) mipHeight /= 2;
					if (mipDepth > 1) mipDepth /= 2;
				}

				barrier.subresourceRange.baseMipLevel = mMipLevel - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(vCommandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);
			}
		}
	}
}