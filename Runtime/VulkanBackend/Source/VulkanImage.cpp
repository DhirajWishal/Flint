// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanImage.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"
#include "VulkanBackend/VulkanImageView.hpp"

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

		VulkanImage::VulkanImage(VulkanDevice* pDevice, const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const uint8_t layers, const uint32_t mipLevels, const void* pImageData, const MultiSampleCount sampleCount)
			: Image(pDevice, type, usage, extent, format, layers, mipLevels, pImageData, sampleCount)
		{
			Initialize(pImageData);
		}

		void VulkanImage::GenerateMipMaps()
		{
			OPTICK_EVENT();

			if (m_MipLevels == 1)
				return;

			VkFormatProperties vProperties = {};
			vkGetPhysicalDeviceFormatProperties(pDevice->GetPhysicalDevice(), Utilities::GetVulkanFormat(m_Format), &vProperties);

			if (!(vProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
				throw backend_error("Texture format does not support linear bitting!");

			if (m_Type == ImageType::CubeMap || m_Type == ImageType::CubeMapArray)
				GenerateCubeMapMipChain();
			else
				GenerateDefaultMipChain();

			vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		std::unique_ptr<VulkanBuffer> VulkanImage::CopyToBuffer()
		{
			std::unique_ptr<VulkanBuffer> pBuffer = std::make_unique<VulkanBuffer>(pDevice, BufferType::Staging, static_cast<uint64_t>(m_Extent.m_Width) * m_Extent.m_Height * m_Extent.m_Depth * Utilities::GetByteDepth(m_Format) * m_LayerCount);

			VkBufferImageCopy vCopy = {};
			vCopy.bufferOffset = 0;
			vCopy.bufferImageHeight = m_Extent.m_Height;
			vCopy.bufferRowLength = m_Extent.m_Width;
			vCopy.imageOffset = { 0, 0, 0 };
			vCopy.imageExtent.width = m_Extent.m_Width;
			vCopy.imageExtent.height = m_Extent.m_Height;
			vCopy.imageExtent.depth = m_Extent.m_Depth;
			vCopy.imageSubresource.aspectMask = Utilities::GetImageAspectFlags(m_Usage);
			vCopy.imageSubresource.baseArrayLayer = 0;
			vCopy.imageSubresource.layerCount = m_LayerCount;
			vCopy.imageSubresource.mipLevel = 0;	// TODO

			{
				VulkanOneTimeCommandBuffer vCommandBuffer(*pDevice);
				VkFormat vFormat = Utilities::GetVulkanFormat(m_Format);

				pDevice->SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vFormat, m_LayerCount, 0, m_MipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

				pDevice->GetDeviceTable().vkCmdCopyImageToBuffer(vCommandBuffer, vImage, vCurrentLayout, pBuffer->GetBuffer(), 1, &vCopy);

				pDevice->SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, vFormat, m_LayerCount, 0, m_MipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			return pBuffer;
		}

		std::unique_ptr<VulkanImageView> VulkanImage::CreateImageView(const uint32_t baseLayerIndex, const uint32_t layerCount, const uint32_t baseMipLevel, const uint32_t mipLevels, const ImageUsage usage)
		{
			return std::make_unique<VulkanImageView>(pDevice, this, baseLayerIndex, layerCount, baseMipLevel, mipLevels, usage);
		}

		void VulkanImage::Terminate()
		{
			pDevice->GetDeviceTable().vkDestroyImageView(pDevice->GetLogicalDevice(), vImageView, nullptr);
			vmaDestroyImage(pDevice->GetVmaAllocator(), vImage, vmaAllocation);

			bIsTerminated = true;
		}

		void VulkanImage::CopyFromImage(VkCommandBuffer vCommandBuffer, VkImage vSrcImage, VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers)
		{
			VkImageCopy vImageCopy = {};
			vImageCopy.extent.width = m_Extent.m_Width;
			vImageCopy.extent.height = m_Extent.m_Height;
			vImageCopy.extent.depth = m_Extent.m_Depth;
			vImageCopy.srcOffset = srcOffset;
			vImageCopy.dstOffset = dstOffset;
			vImageCopy.srcSubresource = subresourceLayers;
			vImageCopy.dstSubresource.layerCount = m_LayerCount;
			vImageCopy.dstSubresource.aspectMask = GetAspectFlags();
			vImageCopy.dstSubresource.baseArrayLayer = 0;
			vImageCopy.dstSubresource.mipLevel = 0; // TODO

			pDevice->GetDeviceTable().vkCmdCopyImage(vCommandBuffer, vSrcImage, vSrcLayout, vImage, vCurrentLayout, 1, &vImageCopy);
		}

		void VulkanImage::CopyFromImage(VkImage vSrcImage, VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers)
		{
			VulkanOneTimeCommandBuffer vCommandBuffer{ *pDevice };
			CopyFromImage(vCommandBuffer, vSrcImage, vSrcLayout, srcOffset, dstOffset, subresourceLayers);
		}

		void VulkanImage::Recreate(const FBox2D& extent)
		{
			if ((m_Usage & ImageUsage::Color) == ImageUsage::Color || (m_Usage & ImageUsage::Depth) == ImageUsage::Depth)
			{
				m_Extent = FBox3D(extent.m_Width, extent.m_Height, m_Extent.m_Depth);

				Terminate();
				Initialize(nullptr);

				bIsTerminated = false;
			}
		}

		VkImageLayout VulkanImage::GetImageLayout(ImageUsage usage) const
		{
			if (usage == ImageUsage(-1))
				return vCurrentLayout;

			if ((usage & m_Usage) != usage)
				throw backend_error("Invalid image usage requested!");

			if (usage == ImageUsage::Graphics && vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				pDevice->SetImageLayout(vImage, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels, 0, GetAspectFlags());
				vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else if (usage == ImageUsage::Storage && vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_GENERAL)
			{
				pDevice->SetImageLayout(vImage, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels, 0, GetAspectFlags());
				vCurrentLayout = VK_IMAGE_LAYOUT_GENERAL;
			}

			return vCurrentLayout;
		}

		VkAttachmentDescription VulkanImage::GetAttachmentDescription() const
		{
			VkAttachmentDescription vDesc = {};
			vDesc.flags = 0;
			vDesc.format = GetImageFormat();
			vDesc.samples = Utilities::GetSampleCount(m_MultiSampleCount);
			vDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			if ((m_Usage & ImageUsage::Graphics) == ImageUsage::Graphics)
				vDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			else if ((m_Usage & ImageUsage::Depth) == ImageUsage::Depth)
				vDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

			else if ((m_Usage & ImageUsage::Color) == ImageUsage::Color)
				vDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			else
				vDesc.finalLayout = vCurrentLayout;

			vDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
			vDesc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
			vDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;

			if (m_ImageRenderingTargetSpecification == ImageRenderTargetSpecification(0))
				return vDesc;

			// Resolve load operation.
			if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::LoadOnLoad)
				vDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::ClearOnLoad)
				vDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::DiscardLoad)
				vDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			// Resolve store operation.
			if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StoreOnStore)
				vDesc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::DiscardStore)
				vDesc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;

			// Resolve stencil load operation.
			if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StencilLoadOnLoad)
				vDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StencilClearOnLoad)
				vDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StencilDiscardLoad)
				vDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			// Resolve stencil store operation.
			if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StencilStoreOnStore)
				vDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StencilDiscardStore)
				vDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;

			return vDesc;
		}

		VkImageLayout VulkanImage::GetAttachmentLayout() const
		{
			if ((m_Usage & ImageUsage::Depth) == ImageUsage::Depth)
				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			if ((m_Usage & ImageUsage::Color) == ImageUsage::Color)
				return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			return vCurrentLayout;
		}

		VkImageView VulkanImage::GetImageView(uint32_t index) const
		{
			return vImageView;
		}

		RenderTargetAttachmenType VulkanImage::GetAttachmentType() const
		{
			return (m_Usage & ImageUsage::Depth) == ImageUsage::Depth ? RenderTargetAttachmenType::DepthBuffer : RenderTargetAttachmenType::ColorBuffer;
		}

		VkFormat VulkanImage::GetImageFormat() const
		{
			return Utilities::GetVulkanFormat(m_Format);
		}

		VkImageViewType VulkanImage::GetImageViewType() const
		{
			return Helpers::GetImageViewType(m_Type);
		}

		VkImageAspectFlags VulkanImage::GetAspectFlags() const
		{
			return Utilities::GetImageAspectFlags(m_Usage);
		}

		VkComponentMapping VulkanImage::GetComponentMapping() const
		{
			return Helpers::GetComponentMapping(m_Format);
		}

		VkImageView VulkanImage::CreateLayerBasedImageView(uint32_t layerNumber) const
		{
			VkImageView vImageView = VK_NULL_HANDLE;

			if (m_Type == ImageType::CubeMap || m_Type == ImageType::CubeMapArray)
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(m_Format), pDevice->StaticCast<VulkanDevice>(), Utilities::GetImageAspectFlags(m_Usage), Helpers::GetImageViewType(m_Type), 1, layerNumber, m_MipLevels, 0, { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A })[0];
			else
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(m_Format), pDevice->StaticCast<VulkanDevice>(), Utilities::GetImageAspectFlags(m_Usage), Helpers::GetImageViewType(m_Type), 1, layerNumber, m_MipLevels, 0)[0];

			return vImageView;
		}

		void VulkanImage::SetImageLayout(VkCommandBuffer vCommandBuffer, VkImageLayout vNewLayout, uint32_t layerCount, uint32_t layerIndex, const uint32_t mipLevels) const
		{
			pDevice->SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, vNewLayout, Utilities::GetVulkanFormat(m_Format), layerCount, layerIndex, mipLevels);
			vCurrentLayout = vNewLayout;
		}

		void VulkanImage::SetImageLayout(VkImageLayout vNewLayout) const
		{
			VulkanOneTimeCommandBuffer vCommandBuffer(*pDevice);
			SetImageLayoutManual(vCommandBuffer, vNewLayout);
		}

		void VulkanImage::SetImageLayoutManual(VkCommandBuffer vCommandBuffer, VkImageLayout vNewLayout) const
		{
			SetImageLayout(vCommandBuffer, vNewLayout, m_LayerCount, 0, m_MipLevels);
			vCurrentLayout = vNewLayout;
		}

		void VulkanImage::CreateImage()
		{
			OPTICK_EVENT();

			vCurrentLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;

			VkImageCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.extent.width = m_Extent.m_Width;
			vCreateInfo.extent.height = m_Extent.m_Height;
			vCreateInfo.extent.depth = m_Extent.m_Depth;
			vCreateInfo.arrayLayers = m_LayerCount;
			vCreateInfo.format = Utilities::GetVulkanFormat(m_Format);
			vCreateInfo.imageType = Helpers::GetImageType(m_Type);
			vCreateInfo.initialLayout = vCurrentLayout;
			vCreateInfo.mipLevels = m_MipLevels;
			vCreateInfo.queueFamilyIndexCount = 0;
			vCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCreateInfo.samples = Utilities::GetSampleCount(m_MultiSampleCount);
			vCreateInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
			vCreateInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
			vCreateInfo.usage = Helpers::GetImageUsage(m_Usage);

			if (m_Type == ImageType::CubeMap || m_Type == ImageType::CubeMapArray)
				vCreateInfo.flags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

			if ((m_Usage & ImageUsage::Color) == ImageUsage::Color)
			{
				VkFormatProperties vProperties = {};
				vkGetPhysicalDeviceFormatProperties(pDevice->GetPhysicalDevice(), Utilities::GetVulkanFormat(m_Format), &vProperties);

				if (!(vProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT))
					throw backend_error("Texture format does not support using as storage image!");
			}

			// Prepare the queue indexes if the device if the compute and graphics queues are different.
			std::vector<uint32_t> queueIndexes = {};
			if (pDevice->IsGraphicsCompatible() && pDevice->IsComputeCompatible())
			{
				if (pDevice->GetQueue().m_GraphicsFamily != pDevice->GetQueue().m_ComputeFamily)
				{
					queueIndexes.emplace_back(pDevice->GetQueue().m_GraphicsFamily.value());
					queueIndexes.emplace_back(pDevice->GetQueue().m_ComputeFamily.value());

					vCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
				}
			}

			vCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueIndexes.size());
			vCreateInfo.pQueueFamilyIndices = queueIndexes.data();

			//FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreateImage(pDevice->GetLogicalDevice(), &vCreateInfo, nullptr, &vImage));

			VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
			vmaAllocationCreateInfo.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
			vmaCreateImage(pDevice->GetVmaAllocator(), &vCreateInfo, &vmaAllocationCreateInfo, &vImage, &vmaAllocation, nullptr);
		}

		void VulkanImage::CreateImageView()
		{
			OPTICK_EVENT();

			if (m_Type == ImageType::CubeMap || m_Type == ImageType::CubeMapArray || m_Usage == ImageUsage::Storage)
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(m_Format), pDevice->StaticCast<VulkanDevice>(), Utilities::GetImageAspectFlags(m_Usage), Helpers::GetImageViewType(m_Type), m_LayerCount, 0, m_MipLevels, 0, Helpers::GetComponentMapping(m_Format))[0];
			else
				vImageView = Utilities::CreateImageViews({ vImage }, Utilities::GetVulkanFormat(m_Format), pDevice->StaticCast<VulkanDevice>(), Utilities::GetImageAspectFlags(m_Usage), Helpers::GetImageViewType(m_Type), m_LayerCount, 0, m_MipLevels, 0)[0];
		}

		void VulkanImage::GenerateDefaultMipChain()
		{
			VulkanOneTimeCommandBuffer vCommandBuffer(*pDevice);
			if (vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				pDevice->SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				vCurrentLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			}

			for (uint32_t i = 0; i < m_LayerCount; i++)
			{
				VkImageMemoryBarrier barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.image = vImage;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.subresourceRange.aspectMask = Utilities::GetImageAspectFlags(m_Usage);
				barrier.subresourceRange.layerCount = m_LayerCount - i;
				barrier.subresourceRange.levelCount = 1;
				barrier.subresourceRange.baseArrayLayer = i;

				int32_t mipWidth = static_cast<int32_t>(m_Extent.m_Width);
				int32_t mipHeight = static_cast<int32_t>(m_Extent.m_Height);
				int32_t mipDepth = static_cast<int32_t>(m_Extent.m_Depth);

				for (uint32_t j = 1; j < m_MipLevels; j++)
				{
					barrier.subresourceRange.baseMipLevel = j - 1;
					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

					pDevice->GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
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
					blit.srcSubresource.layerCount = m_LayerCount;
					blit.dstOffsets[0] = { 0, 0, 0 };
					blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, mipDepth > 1 ? mipDepth / 2 : 1 };
					blit.dstSubresource.aspectMask = barrier.subresourceRange.aspectMask;
					blit.dstSubresource.mipLevel = j;
					blit.dstSubresource.baseArrayLayer = i;
					blit.dstSubresource.layerCount = m_LayerCount - i;

					pDevice->GetDeviceTable().vkCmdBlitImage(vCommandBuffer,
						vImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1, &blit,
						VK_FILTER_LINEAR);

					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

					pDevice->GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
						VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
						0, nullptr,
						0, nullptr,
						1, &barrier);

					if (mipWidth > 1) mipWidth /= 2;
					if (mipHeight > 1) mipHeight /= 2;
					if (mipDepth > 1) mipDepth /= 2;
				}

				barrier.subresourceRange.baseMipLevel = m_MipLevels - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				pDevice->GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);
			}
		}

		void VulkanImage::GenerateCubeMapMipChain()
		{
			VkFormatProperties vProperties = {};
			vkGetPhysicalDeviceFormatProperties(pDevice->GetPhysicalDevice(), Utilities::GetVulkanFormat(m_Format), &vProperties);

			VulkanOneTimeCommandBuffer vCommandBuffer(*pDevice);
			if (vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				pDevice->SetImageLayout(vCommandBuffer, vImage, vCurrentLayout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				vCurrentLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			}

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = vImage;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = Utilities::GetImageAspectFlags(m_Usage);
			barrier.subresourceRange.layerCount = m_LayerCount;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;

			int32_t mipWidth = static_cast<int32_t>(m_Extent.m_Width);
			int32_t mipHeight = static_cast<int32_t>(m_Extent.m_Height);
			int32_t mipDepth = static_cast<int32_t>(m_Extent.m_Depth);

			for (uint32_t j = 1; j < m_MipLevels; j++)
			{
				barrier.subresourceRange.baseMipLevel = j - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				pDevice->GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
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
				blit.srcSubresource.layerCount = m_LayerCount;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, mipDepth > 1 ? mipDepth / 2 : 1 };
				blit.dstSubresource.aspectMask = barrier.subresourceRange.aspectMask;
				blit.dstSubresource.mipLevel = j;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = m_LayerCount;

				pDevice->GetDeviceTable().vkCmdBlitImage(vCommandBuffer,
					vImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				pDevice->GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;
				if (mipDepth > 1) mipDepth /= 2;
			}

			barrier.subresourceRange.baseMipLevel = m_MipLevels - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			pDevice->GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);
		}

		void VulkanImage::Initialize(const void* pImageData)
		{
			OPTICK_EVENT();

			CreateImage();
			CreateImageView();

			// Copy data to the device.
			if (pImageData)
			{
				const uint64_t size = static_cast<uint64_t>(m_Extent.m_Width) * m_Extent.m_Height * m_Extent.m_Depth * Utilities::GetByteDepth(m_Format) * m_LayerCount;
				VulkanBuffer vStagingBuffer(pDevice, BufferType::Staging, size);

				std::byte* pBytes = static_cast<std::byte*>(vStagingBuffer.MapMemory(size));
				std::copy(static_cast<const std::byte*>(pImageData), static_cast<const std::byte*>(pImageData) + size, pBytes);
				vStagingBuffer.UnmapMemory();

				pDevice->SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

				// Copy from buffer. TODO
				{
					VkBufferImageCopy vCopy = {};
					vCopy.bufferImageHeight = 0;
					vCopy.bufferRowLength = 0;
					vCopy.bufferOffset = 0;
					vCopy.imageOffset = { 0, 0, 0 };
					vCopy.imageExtent.width = m_Extent.m_Width;
					vCopy.imageExtent.height = m_Extent.m_Height;
					vCopy.imageExtent.depth = m_Extent.m_Depth;
					vCopy.imageSubresource.aspectMask = Utilities::GetImageAspectFlags(m_Usage);
					vCopy.imageSubresource.baseArrayLayer = 0;
					vCopy.imageSubresource.layerCount = m_LayerCount;
					vCopy.imageSubresource.mipLevel = 0;

					VulkanOneTimeCommandBuffer vCommandBuffer(*pDevice);
					pDevice->GetDeviceTable().vkCmdCopyBufferToImage(vCommandBuffer, vStagingBuffer.GetBuffer(), vImage, vCurrentLayout, 1, &vCopy);
				}

				vStagingBuffer.Terminate();
			}
			if ((m_Usage & ImageUsage::Depth) == ImageUsage::Depth)
			{
				vCurrentLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;	// TODO
			}
			else if ((m_Usage & ImageUsage::Graphics) == ImageUsage::Graphics)
			{
				pDevice->SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else if ((m_Usage & ImageUsage::Color) == ImageUsage::Color)
			{
				pDevice->SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			else if ((m_Usage & ImageUsage::Storage) == ImageUsage::Storage)
			{
				pDevice->SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_GENERAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_GENERAL;
			}
			else
			{
				pDevice->SetImageLayout(vImage, vCurrentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
		}
	}
}