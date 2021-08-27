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
			VkImageType GetImageType(const ImageType type)
			{
				switch (type)
				{
				case Flint::ImageType::DIMENSIONS_1:
				case Flint::ImageType::DIMENSIONS_1_ARRAY:
					return VkImageType::VK_IMAGE_TYPE_1D;

				case Flint::ImageType::DIMENSIONS_2:
				case Flint::ImageType::DIMENSIONS_2_ARRAY:
					return VkImageType::VK_IMAGE_TYPE_2D;

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

			VkImageUsageFlags GetImageUsage(const ImageUsage usage)
			{
				VkImageUsageFlags vFlags = VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;

				if ((usage & ImageUsage::GRAPHICS) == ImageUsage::GRAPHICS)
					vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;

				if ((usage & ImageUsage::STORAGE) == ImageUsage::STORAGE)
					vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;

				if ((usage & ImageUsage::COLOR) == ImageUsage::COLOR)
					vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				if ((usage & ImageUsage::DEPTH) == ImageUsage::DEPTH)
					vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

				return vFlags;
			}

			VkImageViewType GetImageViewType(const ImageType type)
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

			VkImageAspectFlags GetImageAspectFlags(const ImageUsage usage)
			{
				if ((usage & ImageUsage::DEPTH) == ImageUsage::DEPTH)
					return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;

				VkImageAspectFlags vFlags = 0;

				if ((usage & ImageUsage::GRAPHICS) == ImageUsage::GRAPHICS)
					vFlags |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

				if ((usage & ImageUsage::STORAGE) == ImageUsage::STORAGE)
					vFlags |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

				if ((usage & ImageUsage::COLOR) == ImageUsage::COLOR)
					vFlags |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

				return vFlags;
			}

			VkComponentMapping GetComponentMapping(const PixelFormat format)
			{
				VkComponentMapping vComponentMapping = {};
				switch (format)
				{
				case Flint::PixelFormat::UNDEFINED:
				case Flint::PixelFormat::D16_SINT:
				case Flint::PixelFormat::D32_SFLOAT:
				case Flint::PixelFormat::S8_UINT:
				case Flint::PixelFormat::D16_UNORMAL_S8_UINT:
				case Flint::PixelFormat::D24_UNORMAL_S8_UINT:
				case Flint::PixelFormat::D32_SFLOAT_S8_UINT:
					vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
					vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
					vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
					vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
					break;

				case Flint::PixelFormat::R8_SRGB:
				case Flint::PixelFormat::R8_UNORMAL:
				case Flint::PixelFormat::R16_SFLOAT:
				case Flint::PixelFormat::R32_SFLOAT:
					vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					break;

				case Flint::PixelFormat::R8G8_SRGB:
				case Flint::PixelFormat::R8G8_UNORMAL:
				case Flint::PixelFormat::R16G16_SFLOAT:
				case Flint::PixelFormat::R32G32_SFLOAT:
					vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
					vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ZERO;
					vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ZERO;
					break;

				case Flint::PixelFormat::R8G8B8_SRGB:
				case Flint::PixelFormat::R8G8B8_UNORMAL:
				case Flint::PixelFormat::R16G16B16_SFLOAT:
				case Flint::PixelFormat::R32G32B32_SFLOAT:
					vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
					vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
					vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ZERO;
					break;

				case Flint::PixelFormat::R8G8B8A8_SRGB:
				case Flint::PixelFormat::R8G8B8A8_UNORMAL:
				case Flint::PixelFormat::R16G16B16A16_SFLOAT:
				case Flint::PixelFormat::R32G32B32A32_SFLOAT:
					vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
					vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
					vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;
					break;

				case Flint::PixelFormat::B8G8R8_SRGB:
				case Flint::PixelFormat::B8G8R8_UNORMAL:
					vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
					vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
					vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ZERO;
					break;

				case Flint::PixelFormat::B8G8R8A8_SRGB:
				case Flint::PixelFormat::B8G8R8A8_UNORMAL:
					vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
					vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
					vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;
					break;
				}

				return vComponentMapping;
			}
		}

		VulkanImage::VulkanImage(const std::shared_ptr<Device>& pDevice, const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const UI8 layers, const UI32 mipLevels, const void* pImageData, const MultiSampleCount sampleCount)
			: Image(pDevice, type, usage, extent, format, layers, mipLevels, pImageData, sampleCount)
		{
			Initialize(pImageData);
		}

		std::shared_ptr<Buffer> VulkanImage::CopyToBuffer()
		{
			std::shared_ptr<VulkanBuffer> pBuffer = std::make_shared<VulkanBuffer>(pDevice, BufferType::STAGING, static_cast<UI64>(mExtent.mWidth) * mExtent.mHeight * mExtent.mDepth * Utilities::GetByteDepth(mFormat) * mLayerCount);

			VkBufferImageCopy vCopy = {};
			vCopy.bufferOffset = 0;
			vCopy.bufferImageHeight = mExtent.mHeight;
			vCopy.bufferRowLength = mExtent.mWidth;
			vCopy.imageOffset = { 0, 0, 0 };
			vCopy.imageExtent.width = mExtent.mWidth;
			vCopy.imageExtent.height = mExtent.mHeight;
			vCopy.imageExtent.depth = mExtent.mDepth;
			vCopy.imageSubresource.aspectMask = _Helpers::GetImageAspectFlags(mUsage);
			vCopy.imageSubresource.baseArrayLayer = 0;
			vCopy.imageSubresource.layerCount = mLayerCount;
			vCopy.imageSubresource.mipLevel = 0;	// TODO

			{
				VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
				VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);

				vDevice.SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

				vkCmdCopyImageToBuffer(vCommandBuffer, vImage, vCurrentLayout, pBuffer->GetBuffer(), 1, &vCopy);

				vDevice.SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			return pBuffer;
		}

		void VulkanImage::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vkDestroyImage(vDevice.GetLogicalDevice(), vImage, nullptr);
			vkDestroyImageView(vDevice.GetLogicalDevice(), vImageView, nullptr);
			vkFreeMemory(vDevice.GetLogicalDevice(), vImageMemory, nullptr);

			bIsTerminated = true;
		}

		void VulkanImage::CopyFromImage(VkImage vSrcImage, VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers)
		{
			VulkanOneTimeCommandBuffer vCommandBuffer{ pDevice->StaticCast<VulkanDevice>() };

			VkImageCopy vImageCopy = {};
			vImageCopy.extent.width = mExtent.mWidth;
			vImageCopy.extent.height = mExtent.mHeight;
			vImageCopy.extent.depth = mExtent.mDepth;
			vImageCopy.srcOffset = srcOffset;
			vImageCopy.dstOffset = dstOffset;
			vImageCopy.srcSubresource = subresourceLayers;

			vkCmdCopyImage(vCommandBuffer, vSrcImage, vSrcLayout, vImage, vCurrentLayout, 1, &vImageCopy);
		}

		void VulkanImage::Recreate(const FBox2D& extent)
		{
			if ((mUsage & ImageUsage::COLOR) == ImageUsage::COLOR || (mUsage & ImageUsage::DEPTH) == ImageUsage::DEPTH)
			{
				mExtent = FBox3D(extent.mWidth, extent.mHeight, mExtent.mDepth);

				Terminate();
				Initialize(nullptr);

				bIsTerminated = false;
			}
		}

		VkAttachmentDescription VulkanImage::GetAttachmentDescription() const
		{
			VkAttachmentDescription vDesc = {};
			vDesc.flags = 0;
			vDesc.format = GetImageFormat();
			vDesc.samples = Utilities::GetSampleCount(mMultiSampleCount);
			vDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			vDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			vDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			vDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			if ((mUsage & ImageUsage::DEPTH) == ImageUsage::DEPTH)
				vDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			else if ((mUsage & ImageUsage::COLOR) == ImageUsage::COLOR)
				vDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			else
				vDesc.finalLayout = vCurrentLayout;

			return vDesc;
		}

		VkImageLayout VulkanImage::GetAttachmentLayout() const
		{
			if ((mUsage & ImageUsage::DEPTH) == ImageUsage::DEPTH)
				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			if ((mUsage & ImageUsage::COLOR) == ImageUsage::COLOR)
				return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			return vCurrentLayout;
		}

		VkImageView VulkanImage::GetImageView(UI32 index) const
		{
			return vImageView;
		}

		RenderTargetAttachmenType VulkanImage::GetAttachmentType() const
		{
			return (mUsage & ImageUsage::DEPTH) == ImageUsage::DEPTH ? RenderTargetAttachmenType::DEPTH_BUFFER : RenderTargetAttachmenType::COLOR_BUFFER;
		}

		VkFormat VulkanImage::GetImageFormat() const
		{
			return Utilities::GetVulkanFormat(mFormat);
		}

		VkImageView VulkanImage::CreateLayerBasedImageView(UI32 layerNumber) const
		{
			VkImageView vImageView = VK_NULL_HANDLE;

			if (mType == ImageType::CUBEMAP || mType == ImageType::CUBEMAP_ARRAY)
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(mFormat), pDevice->StaticCast<VulkanDevice>(), _Helpers::GetImageAspectFlags(mUsage), _Helpers::GetImageViewType(mType), 1, layerNumber, { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A })[0];
			else
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(mFormat), pDevice->StaticCast<VulkanDevice>(), _Helpers::GetImageAspectFlags(mUsage), _Helpers::GetImageViewType(mType), 1, layerNumber)[0];

			return vImageView;
		}

		void VulkanImage::SetImageLayout(VkCommandBuffer vCommandBuffer, VkImageLayout vNewLayout, UI32 layerCount, UI32 layerIndex, const UI32 mipLevels)
		{
			pDevice->StaticCast<VulkanDevice>().SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, vNewLayout, Utilities::GetVulkanFormat(mFormat), layerCount, layerIndex, mipLevels);
			vCurrentLayout = vNewLayout;
		}

		void VulkanImage::SetImageLayout(VkImageLayout vNewLayout)
		{
			VulkanOneTimeCommandBuffer vCommandBuffer(pDevice->StaticCast<VulkanDevice>());
			SetImageLayout(vCommandBuffer, vNewLayout, mLayerCount, 0, mMipLevels);
		}

		void VulkanImage::CreateImage()
		{
			FLINT_SETUP_PROFILER();

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			vCurrentLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;

			VkImageCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.extent.width = mExtent.mWidth;
			vCreateInfo.extent.height = mExtent.mHeight;
			vCreateInfo.extent.depth = mExtent.mDepth;
			vCreateInfo.arrayLayers = mLayerCount;
			vCreateInfo.format = Utilities::GetVulkanFormat(mFormat);
			vCreateInfo.imageType = _Helpers::GetImageType(mType);
			vCreateInfo.initialLayout = vCurrentLayout;
			vCreateInfo.mipLevels = mMipLevels;
			vCreateInfo.queueFamilyIndexCount = 0;
			vCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCreateInfo.samples = Utilities::GetSampleCount(mMultiSampleCount);
			vCreateInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
			vCreateInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
			vCreateInfo.usage = _Helpers::GetImageUsage(mUsage);

			if (mType == ImageType::CUBEMAP || mType == ImageType::CUBEMAP_ARRAY)
				vCreateInfo.flags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

			if ((mUsage & ImageUsage::COLOR) == ImageUsage::COLOR)
			{
				VkFormatProperties vProperties = {};
				vkGetPhysicalDeviceFormatProperties(vDevice.GetPhysicalDevice(), Utilities::GetVulkanFormat(mFormat), &vProperties);

				if (!(vProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT))
					FLINT_THROW_BACKEND_ERROR("Texture format does not support using as storage image!");
			}

			std::vector<UI32> queueIndexes = {};
			if (vDevice.IsGraphicsCompatible() && vDevice.IsComputeCompatible())
			{
				if (vDevice.GetQueue().mGraphicsFamily != vDevice.GetQueue().mComputeFamily)
				{
					queueIndexes.push_back(vDevice.GetQueue().mGraphicsFamily.value());
					queueIndexes.push_back(vDevice.GetQueue().mComputeFamily.value());

					vCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
				}
			}

			vCreateInfo.queueFamilyIndexCount = static_cast<UI32>(queueIndexes.size());
			vCreateInfo.pQueueFamilyIndices = queueIndexes.data();

			FLINT_VK_ASSERT(vkCreateImage(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vImage));
		}

		void VulkanImage::CreateImageMemory()
		{
			FLINT_SETUP_PROFILER();

			FLINT_VK_ASSERT(pDevice->StaticCast<VulkanDevice>().CreateImageMemory({ vImage }, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vImageMemory));
		}

		void VulkanImage::CreateImageView()
		{
			FLINT_SETUP_PROFILER();

			if (mType == ImageType::CUBEMAP || mType == ImageType::CUBEMAP_ARRAY || mUsage == ImageUsage::STORAGE)
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(mFormat), pDevice->StaticCast<VulkanDevice>(), _Helpers::GetImageAspectFlags(mUsage), _Helpers::GetImageViewType(mType), mLayerCount, 0, _Helpers::GetComponentMapping(mFormat))[0];
			else
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(mFormat), pDevice->StaticCast<VulkanDevice>(), _Helpers::GetImageAspectFlags(mUsage), _Helpers::GetImageViewType(mType), mLayerCount)[0];
		}

		void VulkanImage::GenerateMipMaps()
		{
			FLINT_SETUP_PROFILER();

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			VkFormatProperties vProperties = {};
			vkGetPhysicalDeviceFormatProperties(vDevice.GetPhysicalDevice(), Utilities::GetVulkanFormat(mFormat), &vProperties);

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
				barrier.subresourceRange.aspectMask = _Helpers::GetImageAspectFlags(mUsage);
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
					blit.srcSubresource.aspectMask = barrier.subresourceRange.aspectMask;
					blit.srcSubresource.mipLevel = j - 1;
					blit.srcSubresource.baseArrayLayer = i;
					blit.srcSubresource.layerCount = mLayerCount;
					blit.dstOffsets[0] = { 0, 0, 0 };
					blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, mipDepth > 1 ? mipDepth / 2 : 1 };
					blit.dstSubresource.aspectMask = barrier.subresourceRange.aspectMask;
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

			vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		
		void VulkanImage::Initialize(const void* pImageData)
		{
			FLINT_SETUP_PROFILER();
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			CreateImage();
			CreateImageMemory();
			CreateImageView();

			// Copy date to the device.
			if (pImageData)
			{
				const UI64 size = static_cast<UI64>(mExtent.mWidth) * mExtent.mHeight * mExtent.mDepth * Utilities::GetByteDepth(mFormat) * mLayerCount;
				VulkanBuffer vStagingBuffer(pDevice, BufferType::STAGING, size);

				BYTE* pBytes = static_cast<BYTE*>(vStagingBuffer.MapMemory(size));
				std::copy(static_cast<const BYTE*>(pImageData), static_cast<const BYTE*>(pImageData) + size, pBytes);
				vStagingBuffer.UnmapMemory();

				vDevice.SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
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
					vCopy.imageSubresource.aspectMask = _Helpers::GetImageAspectFlags(mUsage);
					vCopy.imageSubresource.baseArrayLayer = 0;
					vCopy.imageSubresource.layerCount = mLayerCount;
					vCopy.imageSubresource.mipLevel = 0;

					VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);
					vkCmdCopyBufferToImage(vCommandBuffer, vStagingBuffer.GetBuffer(), vImage, vCurrentLayout, 1, &vCopy);
				}

				vStagingBuffer.Terminate();
			}

			if (mType != ImageType::CUBEMAP && mType != ImageType::CUBEMAP_ARRAY && mUsage == ImageUsage::GRAPHICS)
				GenerateMipMaps();
			else if ((mUsage & ImageUsage::DEPTH) == ImageUsage::DEPTH)
			{
				vCurrentLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;	// TODO
			}
			else if ((mUsage & ImageUsage::GRAPHICS) == ImageUsage::GRAPHICS)
			{
				vDevice.SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else if ((mUsage & ImageUsage::COLOR) == ImageUsage::COLOR)
			{
				vDevice.SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			else if ((mUsage & ImageUsage::STORAGE) == ImageUsage::STORAGE)
			{
				vDevice.SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_GENERAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_GENERAL;
			}
			else
			{
				vDevice.SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
		}
	}
}