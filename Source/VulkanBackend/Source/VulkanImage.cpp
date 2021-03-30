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
		}

		void VulkanImage::Initialize(DeviceType* pDevice, UI64 width, UI64 height, UI64 depth, Backend::ImageUsage usage, UI8 bitsPerPixel, UI8 layers)
		{
			this->pDevice = pDevice;
			this->mWidth = width;
			this->mHeight = height;
			this->mDepth = depth;
			this->mUsage = usage;
			this->mBitsPerPixel = bitsPerPixel;
			this->mLayers = layers;
			this->mMipLevel = static_cast<UI32>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1;
			this->vFormat = _Helpers::GetFormat(bitsPerPixel);

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

			FLINT_VK_ASSERT(pDevice->CreateImage(&vCI, &vImage), "Failed to create image!")
				FLINT_VK_ASSERT(pDevice->CreateImageMemory({ vImage }, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vImageMemory), "Failed to allocate image memory!")

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

			FLINT_VK_ASSERT(pDevice->CreateImageView(&vIVCI, &vImageView), "Failed to create image view!")
		}

		void VulkanImage::Terminate()
		{
			pDevice->DestroyImage(vImage);
			pDevice->DestroyImageView(vImageView);
			pDevice->FreeMemory(vImageMemory);
		}

		void VulkanImage::CopyData(unsigned char* pData)
		{
			SetImageLayout(VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			UI64 size = mWidth * mHeight * (mBitsPerPixel / 8);

			VulkanBuffer staggingBuffer = {};
			staggingBuffer.Initialize(pDevice, size, Backend::BufferUsage::STAGGING, Backend::MemoryProfile::TRANSFER_FRIENDLY);

			BYTE* pDataStore = static_cast<BYTE*>(staggingBuffer.MapMemory(size, 0));
			std::copy(pData, pData + size, pDataStore);

			staggingBuffer.FlushMemoryMappings();
			staggingBuffer.UnmapMemory();

			VkBufferImageCopy vBIC = {};
			vBIC.bufferOffset = 0;
			vBIC.bufferImageHeight = 0;
			vBIC.bufferRowLength = 0;
			vBIC.imageOffset = { 0, 0, 0 };
			vBIC.imageSubresource.baseArrayLayer = 0;
			vBIC.imageSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			vBIC.imageExtent.width = static_cast<UI32>(mWidth);
			vBIC.imageExtent.height = static_cast<UI32>(mHeight);
			vBIC.imageExtent.depth = static_cast<UI32>(mDepth);
			vBIC.imageSubresource.layerCount = mLayers;
			vBIC.imageSubresource.mipLevel = mMipLevel;

			VulkanOneTimeCommandBuffer vCommandBuffer(pDevice);
			vkCmdCopyBufferToImage(vCommandBuffer, staggingBuffer.vBuffer, vImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vBIC);
		}

		void VulkanImage::SetImageLayout(VkImageLayout layout)
		{
			VulkanOneTimeCommandBuffer vCommandBuffer(pDevice);

			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = vCurrentLayout;
			barrier.newLayout = layout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = vImage;

			if (layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				if (Utilities::HasStencilComponent(vFormat))
					barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			else
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = mMipLevel;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = mLayers;
			barrier.srcAccessMask = 0; // TODO
			barrier.dstAccessMask = 0; // TODO

			VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

			switch (barrier.oldLayout)
			{
			case VK_IMAGE_LAYOUT_UNDEFINED:
				//sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				barrier.srcAccessMask = 0;
				break;

			case VK_IMAGE_LAYOUT_PREINITIALIZED:
				barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				//sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				//destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;
			default:
				FLINT_LOG_ERROR(TEXT("Unsupported layout transition!"))
					break;
			}

			switch (barrier.newLayout)
			{
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				//destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				//destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				//destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				barrier.dstAccessMask = barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				if (barrier.srcAccessMask == 0)
					barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;

				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				//destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;

			case VK_IMAGE_LAYOUT_GENERAL:
				//destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
				break;
			default:
				FLINT_LOG_ERROR(TEXT("Unsupported layout transition!"))
					break;
			}

			vkCmdPipelineBarrier(
				vCommandBuffer,
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			vCurrentLayout = layout;
		}

		void VulkanImage::GenerateMipMaps()
		{
			VkFormatProperties vProps = pDevice->GetFormatProperties(vFormat);

			if (!(vProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
				FLINT_LOG_ERROR(TEXT("Texture format does not support linear bitting!"))

				VulkanOneTimeCommandBuffer vCommandBuffer(pDevice);

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