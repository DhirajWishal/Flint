// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanImage.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			VkImageType GetImageType(ImageType type)
			{
				switch (type)
				{
				case Flint::ImageType::DIMENSIONS_1:
				case Flint::ImageType::DIMENSIONS_1_ARRAY:
					return VkImageType::VK_IMAGE_TYPE_1D;

				case Flint::ImageType::DIMENSIONS_2:
				case Flint::ImageType::DIMENSIONS_2_ARRAY:
				case Flint::ImageType::CUBEMAP:
				case Flint::ImageType::CUBEMAP_ARRAY:
					return VkImageType::VK_IMAGE_TYPE_2D;

				case Flint::ImageType::DIMENSIONS_3:
				case Flint::ImageType::DIMENSIONS_3_ARRAY:
					return VkImageType::VK_IMAGE_TYPE_3D;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid image type!");
				}

				return VkImageType::VK_IMAGE_TYPE_2D;
			}

			VkFormat GetImageFormat(PixelFormat format)
			{
				switch (format)
				{
				case Flint::PixelFormat::R8_SRGB:
					return VkFormat::VK_FORMAT_R8_SRGB;

				case Flint::PixelFormat::R8G8_SRGB:
					return VkFormat::VK_FORMAT_R8G8_SRGB;

				case Flint::PixelFormat::R8G8B8_SRGB:
					return VkFormat::VK_FORMAT_R8G8B8_SRGB;

				case Flint::PixelFormat::R8G8B8A8_SRGB:
					return VkFormat::VK_FORMAT_R8G8B8A8_SRGB;

				case Flint::PixelFormat::B8G8R8_SRGB:
					return VkFormat::VK_FORMAT_B8G8R8_SRGB;

				case Flint::PixelFormat::B8G8R8A8_SRGB:
					return VkFormat::VK_FORMAT_B8G8R8A8_SRGB;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid or undefined pixel format!");
				}

				return VkFormat::VK_FORMAT_UNDEFINED;
			}

			VkImageUsageFlags GetImageUsage(ImageUsage usage)
			{
				switch (usage)
				{
				case Flint::ImageUsage::GRAPHICS:
					return VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;

				case Flint::ImageUsage::STORAGE:
					return VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid image usage type!");
				}

				return VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
			}

			UI8 GetByteDepth(PixelFormat format)
			{
				switch (format)
				{
				case Flint::PixelFormat::R8_SRGB:
					return 1;

				case Flint::PixelFormat::R8G8_SRGB:
					return 2;

				case Flint::PixelFormat::R8G8B8_SRGB:
					return 3;

				case Flint::PixelFormat::R8G8B8A8_SRGB:
					return 4;

				case Flint::PixelFormat::B8G8R8_SRGB:
					return 3;

				case Flint::PixelFormat::B8G8R8A8_SRGB:
					return 4;
				}

				return 0;
			}

			VkImageViewType GetImageViewType(ImageType type)
			{
				switch (type)
				{
				case Flint::ImageType::DIMENSIONS_1:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;

				case Flint::ImageType::DIMENSIONS_1_ARRAY:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY;

				case Flint::ImageType::DIMENSIONS_2:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

				case Flint::ImageType::DIMENSIONS_2_ARRAY:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;

				case Flint::ImageType::CUBEMAP:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
				case Flint::ImageType::CUBEMAP_ARRAY:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;


				case Flint::ImageType::DIMENSIONS_3:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

				case Flint::ImageType::DIMENSIONS_3_ARRAY:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid image type!");
				}

				return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
			}
		}

		VulkanImage::VulkanImage(const std::shared_ptr<Device>& pDevice, ImageType type, ImageUsage usage, const FBox3D& extent, PixelFormat format, UI8 layers, UI32 mipLevels, const void* pImageData)
			: Image(pDevice, type, usage, extent, format, layers, mipLevels, pImageData)
		{
			CreateImage();
			CreateImageMemory();
			CreateImageView();

			// Copy date to the device.
			UI64 size = static_cast<UI64>(mExtent.mWidth) * mExtent.mHeight * mExtent.mDepth * _Helpers::GetByteDepth(mFormat) * mLayerCount;
			VulkanBuffer vStaggingBuffer(pDevice, BufferType::STAGGING, size);

			BYTE* pBytes = static_cast<BYTE*>(vStaggingBuffer.MapMemory(size));
			std::copy(static_cast<const BYTE*>(pImageData), static_cast<const BYTE*>(pImageData) + size, pBytes);
			vStaggingBuffer.UnmapMemory();

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vDevice.SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _Helpers::GetImageFormat(mFormat), mLayerCount, 0, mMipLevels);
			vCurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

			// Copy from buffer. TODO
			{
				VkBufferImageCopy vCopy = {};
				vCopy.bufferImageHeight = 0;
				vCopy.bufferRowLength = 0;
				vCopy.bufferOffset = 0;
				vCopy.imageOffset = { 0, 0, 0 };
				vCopy.imageExtent.width = mExtent.mWidth;
				vCopy.imageExtent.height = mExtent.mHeight;
				vCopy.imageExtent.depth = mExtent.mDepth;
				vCopy.imageSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
				vCopy.imageSubresource.baseArrayLayer = 0;
				vCopy.imageSubresource.layerCount = mLayerCount;
				vCopy.imageSubresource.mipLevel = 0;

				VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);
				vkCmdCopyBufferToImage(vCommandBuffer, vStaggingBuffer.GetBuffer(), vImage, vCurrentLayout, 1, &vCopy);
			}

			vStaggingBuffer.Terminate();

			if (mType != ImageType::CUBEMAP && mType != ImageType::CUBEMAP_ARRAY)
				GenerateMipMaps();
			else
				vDevice.SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, _Helpers::GetImageFormat(mFormat), mLayerCount, 0, mMipLevels);

			vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		void VulkanImage::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vkDestroyImage(vDevice.GetLogicalDevice(), vImage, nullptr);
			vkDestroyImageView(vDevice.GetLogicalDevice(), vImageView, nullptr);
			vkFreeMemory(vDevice.GetLogicalDevice(), vImageMemory, nullptr);
		}

		void VulkanImage::CreateImage()
		{
			VkImageCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.extent.width = mExtent.mWidth;
			vCreateInfo.extent.height = mExtent.mHeight;
			vCreateInfo.extent.depth = mExtent.mDepth;
			vCreateInfo.arrayLayers = mLayerCount;
			vCreateInfo.format = _Helpers::GetImageFormat(mFormat);
			vCreateInfo.imageType = _Helpers::GetImageType(mType);
			vCreateInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
			vCreateInfo.mipLevels = mMipLevels;
			vCreateInfo.queueFamilyIndexCount = 0;
			vCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			vCreateInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
			vCreateInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
			vCreateInfo.usage = _Helpers::GetImageUsage(mUsage);

			if (mType == ImageType::CUBEMAP || mType == ImageType::CUBEMAP_ARRAY)
				vCreateInfo.flags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

			FLINT_VK_ASSERT(vkCreateImage(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCreateInfo, nullptr, &vImage));
		}

		void VulkanImage::CreateImageMemory()
		{
			FLINT_VK_ASSERT(pDevice->StaticCast<VulkanDevice>().CreateImageMemory({ vImage }, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vImageMemory));
		}

		void VulkanImage::CreateImageView()
		{
			if (mType == ImageType::CUBEMAP || mType == ImageType::CUBEMAP_ARRAY)
				vImageView = Utilities::CreateImageViews({ vImage }, _Helpers::GetImageFormat(mFormat), pDevice->StaticCast<VulkanDevice>(), VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, _Helpers::GetImageViewType(mType), mLayerCount, { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A })[0];
			else
				vImageView = Utilities::CreateImageViews({ vImage }, _Helpers::GetImageFormat(mFormat), pDevice->StaticCast<VulkanDevice>(), VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, _Helpers::GetImageViewType(mType), mLayerCount)[0];
		}

		void VulkanImage::GenerateMipMaps()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			VkFormatProperties vProperties = {};
			vkGetPhysicalDeviceFormatProperties(vDevice.GetPhysicalDevice(), _Helpers::GetImageFormat(mFormat), &vProperties);

			if (!(vProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
				FLINT_THROW_BACKEND_ERROR("Texture format does not support linear bitting!");

			VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);

			for (UI32 i = 0; i < mLayerCount; i++)
			{
				VkImageMemoryBarrier barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.image = vImage;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.layerCount = mLayerCount - i;
				barrier.subresourceRange.levelCount = 1;
				barrier.subresourceRange.baseArrayLayer = i;

				I32 mipWidth = static_cast<I32>(mExtent.mWidth);
				I32 mipHeight = static_cast<I32>(mExtent.mHeight);
				I32 mipDepth = static_cast<I32>(mExtent.mDepth);

				for (UI32 j = 1; j < mMipLevels; j++)
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
					blit.srcSubresource.layerCount = mLayerCount;
					blit.dstOffsets[0] = { 0, 0, 0 };
					blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, mipDepth > 1 ? mipDepth / 2 : 1 };
					blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.dstSubresource.mipLevel = j;
					blit.dstSubresource.baseArrayLayer = i;
					blit.dstSubresource.layerCount = mLayerCount - i;

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

				barrier.subresourceRange.baseMipLevel = mMipLevels - 1;
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

/*
Vulkan Validation Layer (Validation): Validation Error: [ VUID-VkImageMemoryBarrier-subresourceRange-01725 ] Object 0: handle = 0x7dceed0000000068, type = VK_OBJECT_TYPE_IMAGE; | MessageID = 0xe9dbfc10 | vkCmdPipelineBarrier(): pImageMemoryBarriers[0].subresourceRange.baseArrayLayer + .layerCount (= 1 + 6 = 7) is greater than the arrayLayers of the image when it was created (i.e. greater than 6). The Vulkan spec states: If subresourceRange.layerCount is not VK_REMAINING_ARRAY_LAYERS, subresourceRange.baseArrayLayer + subresourceRange.layerCount must be less than or equal to the arrayLayers specified in VkImageCreateInfo when image was created (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-VkImageMemoryBarrier-subresourceRange-01725)
Vulkan Validation Layer (Validation): Validation Error: [ VUID-VkImageMemoryBarrier-subresourceRange-01725 ] Object 0: handle = 0x7dceed0000000068, type = VK_OBJECT_TYPE_IMAGE; | MessageID = 0xe9dbfc10 | vkCmdPipelineBarrier(): pImageMemoryBarriers[0].subresourceRange.baseArrayLayer + .layerCount (= 2 + 6 = 8) is greater than the arrayLayers of the image when it was created (i.e. greater than 6). The Vulkan spec states: If subresourceRange.layerCount is not VK_REMAINING_ARRAY_LAYERS, subresourceRange.baseArrayLayer + subresourceRange.layerCount must be less than or equal to the arrayLayers specified in VkImageCreateInfo when image was created (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-VkImageMemoryBarrier-subresourceRange-01725)
Vulkan Validation Layer (Validation): Validation Error: [ VUID-VkImageMemoryBarrier-subresourceRange-01725 ] Object 0: handle = 0x7dceed0000000068, type = VK_OBJECT_TYPE_IMAGE; | MessageID = 0xe9dbfc10 | vkCmdPipelineBarrier(): pImageMemoryBarriers[0].subresourceRange.baseArrayLayer + .layerCount (= 3 + 6 = 9) is greater than the arrayLayers of the image when it was created (i.e. greater than 6). The Vulkan spec states: If subresourceRange.layerCount is not VK_REMAINING_ARRAY_LAYERS, subresourceRange.baseArrayLayer + subresourceRange.layerCount must be less than or equal to the arrayLayers specified in VkImageCreateInfo when image was created (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-VkImageMemoryBarrier-subresourceRange-01725)
Vulkan Validation Layer (Validation): Validation Error: [ VUID-VkImageMemoryBarrier-subresourceRange-01725 ] Object 0: handle = 0x7dceed0000000068, type = VK_OBJECT_TYPE_IMAGE; | MessageID = 0xe9dbfc10 | vkCmdPipelineBarrier(): pImageMemoryBarriers[0].subresourceRange.baseArrayLayer + .layerCount (= 4 + 6 = 10) is greater than the arrayLayers of the image when it was created (i.e. greater than 6). The Vulkan spec states: If subresourceRange.layerCount is not VK_REMAINING_ARRAY_LAYERS, subresourceRange.baseArrayLayer + subresourceRange.layerCount must be less than or equal to the arrayLayers specified in VkImageCreateInfo when image was created (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-VkImageMemoryBarrier-subresourceRange-01725)
Vulkan Validation Layer (Validation): Validation Error: [ VUID-VkImageMemoryBarrier-subresourceRange-01725 ] Object 0: handle = 0x7dceed0000000068, type = VK_OBJECT_TYPE_IMAGE; | MessageID = 0xe9dbfc10 | vkCmdPipelineBarrier(): pImageMemoryBarriers[0].subresourceRange.baseArrayLayer + .layerCount (= 5 + 6 = 11) is greater than the arrayLayers of the image when it was created (i.e. greater than 6). The Vulkan spec states: If subresourceRange.layerCount is not VK_REMAINING_ARRAY_LAYERS, subresourceRange.baseArrayLayer + subresourceRange.layerCount must be less than or equal to the arrayLayers specified in VkImageCreateInfo when image was created (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-VkImageMemoryBarrier-subresourceRange-01725)
*/