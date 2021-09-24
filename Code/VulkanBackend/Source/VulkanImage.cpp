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
		namespace Helpers
		{
			VkImageType GetImageType(const ImageType type)
			{
				switch (type)
				{
				case Flint::ImageType::OneDimension:
				case Flint::ImageType::OneDimensionArray:
					return VkImageType::VK_IMAGE_TYPE_1D;

				case Flint::ImageType::TwoDimension:
				case Flint::ImageType::TwoDimensionArray:
					return VkImageType::VK_IMAGE_TYPE_2D;

				case Flint::ImageType::CubeMap:
				case Flint::ImageType::CubeMapArray:
					return VkImageType::VK_IMAGE_TYPE_2D;

				case Flint::ImageType::ThreeDimension:
				case Flint::ImageType::ThreeDimensionArray:
					return VkImageType::VK_IMAGE_TYPE_3D;

				default:
					throw backend_error("Invalid image type!");
				}

				return VkImageType::VK_IMAGE_TYPE_2D;
			}

			VkImageUsageFlags GetImageUsage(const ImageUsage usage)
			{
				VkImageUsageFlags vFlags = VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;

				if ((usage & ImageUsage::Graphics) == ImageUsage::Graphics)
					vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;

				if ((usage & ImageUsage::Storage) == ImageUsage::Storage)
					vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;

				if ((usage & ImageUsage::Color) == ImageUsage::Color)
					vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				if ((usage & ImageUsage::Depth) == ImageUsage::Depth)
					vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

				return vFlags;
			}

			VkImageViewType GetImageViewType(const ImageType type)
			{
				switch (type)
				{
				case Flint::ImageType::OneDimension:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;

				case Flint::ImageType::OneDimensionArray:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY;

				case Flint::ImageType::TwoDimension:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

				case Flint::ImageType::TwoDimensionArray:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;

				case Flint::ImageType::CubeMap:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;

				case Flint::ImageType::CubeMapArray:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

				case Flint::ImageType::ThreeDimension:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

				case Flint::ImageType::ThreeDimensionArray:
					return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;

				default:
					throw backend_error("Invalid image type!");
				}

				return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
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

			VkComponentMapping GetComponentMapping(const PixelFormat format)
			{
				VkComponentMapping vComponentMapping = {};
				switch (format)
				{
				case Flint::PixelFormat::Undefined:
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

		void VulkanImage::GenerateMipMaps()
		{
			FLINT_SETUP_PROFILER();

			if (mMipLevels == 1)
				return;

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			VkFormatProperties vProperties = {};
			vkGetPhysicalDeviceFormatProperties(vDevice.GetPhysicalDevice(), Utilities::GetVulkanFormat(mFormat), &vProperties);

			if (!(vProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
				throw backend_error("Texture format does not support linear bitting!");

			if (mType == ImageType::CubeMap || mType == ImageType::CubeMapArray)
				GenerateCubeMapMipChain();
			else
				GenerateDefaultMipChain();

			vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		std::shared_ptr<Buffer> VulkanImage::CopyToBuffer()
		{
			std::shared_ptr<VulkanBuffer> pBuffer = std::make_shared<VulkanBuffer>(pDevice, BufferType::Staging, static_cast<UI64>(mExtent.mWidth) * mExtent.mHeight * mExtent.mDepth * Utilities::GetByteDepth(mFormat) * mLayerCount);

			VkBufferImageCopy vCopy = {};
			vCopy.bufferOffset = 0;
			vCopy.bufferImageHeight = mExtent.mHeight;
			vCopy.bufferRowLength = mExtent.mWidth;
			vCopy.imageOffset = { 0, 0, 0 };
			vCopy.imageExtent.width = mExtent.mWidth;
			vCopy.imageExtent.height = mExtent.mHeight;
			vCopy.imageExtent.depth = mExtent.mDepth;
			vCopy.imageSubresource.aspectMask = Helpers::GetImageAspectFlags(mUsage);
			vCopy.imageSubresource.baseArrayLayer = 0;
			vCopy.imageSubresource.layerCount = mLayerCount;
			vCopy.imageSubresource.mipLevel = 0;	// TODO

			{
				VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
				VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);
				VkFormat vFormat = Utilities::GetVulkanFormat(mFormat);

				vDevice.SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vFormat, mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

				vDevice.GetDeviceTable().vkCmdCopyImageToBuffer(vCommandBuffer, vImage, vCurrentLayout, pBuffer->GetBuffer(), 1, &vCopy);

				vDevice.SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, vFormat, mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			return pBuffer;
		}

		void VulkanImage::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkDestroyImage(vDevice.GetLogicalDevice(), vImage, nullptr);
			vDevice.GetDeviceTable().vkDestroyImageView(vDevice.GetLogicalDevice(), vImageView, nullptr);
			vDevice.GetDeviceTable().vkFreeMemory(vDevice.GetLogicalDevice(), vImageMemory, nullptr);

			bIsTerminated = true;
		}

		void VulkanImage::CopyFromImage(VkImage vSrcImage, VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers)
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			VulkanOneTimeCommandBuffer vCommandBuffer{ vDevice };

			VkImageCopy vImageCopy = {};
			vImageCopy.extent.width = mExtent.mWidth;
			vImageCopy.extent.height = mExtent.mHeight;
			vImageCopy.extent.depth = mExtent.mDepth;
			vImageCopy.srcOffset = srcOffset;
			vImageCopy.dstOffset = dstOffset;
			vImageCopy.srcSubresource = subresourceLayers;

			vDevice.GetDeviceTable().vkCmdCopyImage(vCommandBuffer, vSrcImage, vSrcLayout, vImage, vCurrentLayout, 1, &vImageCopy);
		}

		void VulkanImage::Recreate(const FBox2D& extent)
		{
			if ((mUsage & ImageUsage::Color) == ImageUsage::Color || (mUsage & ImageUsage::Depth) == ImageUsage::Depth)
			{
				mExtent = FBox3D(extent.mWidth, extent.mHeight, mExtent.mDepth);

				Terminate();
				Initialize(nullptr);

				bIsTerminated = false;
			}
		}

		VkImageLayout VulkanImage::GetImageLayout(ImageUsage usage) const
		{
			if (usage == ImageUsage(-1))
				return vCurrentLayout;

			if ((usage & mUsage) != usage)
				throw backend_error("Invalid image usage requested!");

			if (usage == ImageUsage::Graphics && vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				pDevice->StaticCast<VulkanDevice>().SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else if (usage == ImageUsage::Storage && vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_GENERAL)
			{
				pDevice->StaticCast<VulkanDevice>().SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_GENERAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_GENERAL;
			}

			return vCurrentLayout;
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

			if ((mUsage & ImageUsage::Graphics) == ImageUsage::Graphics)
				vDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			else if ((mUsage & ImageUsage::Depth) == ImageUsage::Depth)
				vDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

			else if ((mUsage & ImageUsage::Color) == ImageUsage::Color)
				vDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			else
				vDesc.finalLayout = vCurrentLayout;

			return vDesc;
		}

		VkImageLayout VulkanImage::GetAttachmentLayout() const
		{
			if ((mUsage & ImageUsage::Depth) == ImageUsage::Depth)
				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			if ((mUsage & ImageUsage::Color) == ImageUsage::Color)
				return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			return vCurrentLayout;
		}

		VkImageView VulkanImage::GetImageView(UI32 index) const
		{
			return vImageView;
		}

		RenderTargetAttachmenType VulkanImage::GetAttachmentType() const
		{
			return (mUsage & ImageUsage::Depth) == ImageUsage::Depth ? RenderTargetAttachmenType::DepthBuffer : RenderTargetAttachmenType::ColorBuffer;
		}

		VkFormat VulkanImage::GetImageFormat() const
		{
			return Utilities::GetVulkanFormat(mFormat);
		}

		VkImageView VulkanImage::CreateLayerBasedImageView(UI32 layerNumber) const
		{
			VkImageView vImageView = VK_NULL_HANDLE;

			if (mType == ImageType::CubeMap || mType == ImageType::CubeMapArray)
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(mFormat), pDevice->StaticCast<VulkanDevice>(), Helpers::GetImageAspectFlags(mUsage), Helpers::GetImageViewType(mType), 1, layerNumber, mMipLevels, 0, { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A })[0];
			else
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(mFormat), pDevice->StaticCast<VulkanDevice>(), Helpers::GetImageAspectFlags(mUsage), Helpers::GetImageViewType(mType), 1, layerNumber, mMipLevels, 0)[0];

			return vImageView;
		}

		void VulkanImage::SetImageLayout(VkCommandBuffer vCommandBuffer, VkImageLayout vNewLayout, UI32 layerCount, UI32 layerIndex, const UI32 mipLevels) const
		{
			pDevice->StaticCast<VulkanDevice>().SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, vNewLayout, Utilities::GetVulkanFormat(mFormat), layerCount, layerIndex, mipLevels);
			vCurrentLayout = vNewLayout;
		}

		void VulkanImage::SetImageLayout(VkImageLayout vNewLayout) const
		{
			VulkanOneTimeCommandBuffer vCommandBuffer(pDevice->StaticCast<VulkanDevice>());
			SetImageLayout(vCommandBuffer, vNewLayout, mLayerCount, 0, mMipLevels);
			vCurrentLayout = vNewLayout;
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
			vCreateInfo.imageType = Helpers::GetImageType(mType);
			vCreateInfo.initialLayout = vCurrentLayout;
			vCreateInfo.mipLevels = mMipLevels;
			vCreateInfo.queueFamilyIndexCount = 0;
			vCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCreateInfo.samples = Utilities::GetSampleCount(mMultiSampleCount);
			vCreateInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
			vCreateInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
			vCreateInfo.usage = Helpers::GetImageUsage(mUsage);

			if (mType == ImageType::CubeMap || mType == ImageType::CubeMapArray)
				vCreateInfo.flags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

			if ((mUsage & ImageUsage::Color) == ImageUsage::Color)
			{
				VkFormatProperties vProperties = {};
				vkGetPhysicalDeviceFormatProperties(vDevice.GetPhysicalDevice(), Utilities::GetVulkanFormat(mFormat), &vProperties);

				if (!(vProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT))
					throw backend_error("Texture format does not support using as storage image!");
			}

			// Prepare the queue indexes if the device if the compute and graphics queues are different.
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

			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateImage(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vImage));
		}

		void VulkanImage::CreateImageMemory()
		{
			FLINT_SETUP_PROFILER();

			FLINT_VK_ASSERT(pDevice->StaticCast<VulkanDevice>().CreateImageMemory({ vImage }, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vImageMemory));
		}

		void VulkanImage::CreateImageView()
		{
			FLINT_SETUP_PROFILER();

			if (mType == ImageType::CubeMap || mType == ImageType::CubeMapArray || mUsage == ImageUsage::Storage)
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(mFormat), pDevice->StaticCast<VulkanDevice>(), Helpers::GetImageAspectFlags(mUsage), Helpers::GetImageViewType(mType), mLayerCount, 0, mMipLevels, 0, Helpers::GetComponentMapping(mFormat))[0];
			else
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(mFormat), pDevice->StaticCast<VulkanDevice>(), Helpers::GetImageAspectFlags(mUsage), Helpers::GetImageViewType(mType), mLayerCount, 0, mMipLevels, 0)[0];
		}

		void VulkanImage::GenerateDefaultMipChain()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);
			if (vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				vDevice.SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			}

			for (UI32 i = 0; i < mLayerCount; i++)
			{
				VkImageMemoryBarrier barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.image = vImage;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.subresourceRange.aspectMask = Helpers::GetImageAspectFlags(mUsage);
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

					vDevice.GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
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

					vDevice.GetDeviceTable().vkCmdBlitImage(vCommandBuffer,
						vImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1, &blit,
						VK_FILTER_LINEAR);

					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

					vDevice.GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
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

				vDevice.GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);
			}
		}

		void VulkanImage::GenerateCubeMapMipChain()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			VkFormatProperties vProperties = {};
			vkGetPhysicalDeviceFormatProperties(vDevice.GetPhysicalDevice(), Utilities::GetVulkanFormat(mFormat), &vProperties);

			VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);
			if (vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				vDevice.SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			}

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = vImage;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = Helpers::GetImageAspectFlags(mUsage);
			barrier.subresourceRange.layerCount = mLayerCount;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;

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

				vDevice.GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				VkImageBlit blit{};
				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { mipWidth, mipHeight, mipDepth };
				blit.srcSubresource.aspectMask = barrier.subresourceRange.aspectMask;
				blit.srcSubresource.mipLevel = j - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = mLayerCount;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, mipDepth > 1 ? mipDepth / 2 : 1 };
				blit.dstSubresource.aspectMask = barrier.subresourceRange.aspectMask;
				blit.dstSubresource.mipLevel = j;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = mLayerCount;

				vDevice.GetDeviceTable().vkCmdBlitImage(vCommandBuffer,
					vImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vDevice.GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
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

			vDevice.GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);
		}

		void VulkanImage::Initialize(const void* pImageData)
		{
			FLINT_SETUP_PROFILER();
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			CreateImage();
			CreateImageMemory();
			CreateImageView();

			// Copy data to the device.
			if (pImageData)
			{
				const UI64 size = static_cast<UI64>(mExtent.mWidth) * mExtent.mHeight * mExtent.mDepth * Utilities::GetByteDepth(mFormat) * mLayerCount;
				VulkanBuffer vStagingBuffer(pDevice, BufferType::Staging, size);

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
					vCopy.imageSubresource.aspectMask = Helpers::GetImageAspectFlags(mUsage);
					vCopy.imageSubresource.baseArrayLayer = 0;
					vCopy.imageSubresource.layerCount = mLayerCount;
					vCopy.imageSubresource.mipLevel = 0;

					VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);
					vDevice.GetDeviceTable().vkCmdCopyBufferToImage(vCommandBuffer, vStagingBuffer.GetBuffer(), vImage, vCurrentLayout, 1, &vCopy);
				}

				vStagingBuffer.Terminate();
			}
			if ((mUsage & ImageUsage::Depth) == ImageUsage::Depth)
			{
				vCurrentLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;	// TODO
			}
			else if ((mUsage & ImageUsage::Graphics) == ImageUsage::Graphics)
			{
				vDevice.SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else if ((mUsage & ImageUsage::Color) == ImageUsage::Color)
			{
				vDevice.SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, Utilities::GetVulkanFormat(mFormat), mLayerCount, 0, mMipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			else if ((mUsage & ImageUsage::Storage) == ImageUsage::Storage)
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