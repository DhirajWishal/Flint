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
				VkImageUsageFlags m_vFlags = VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;

				if ((usage & ImageUsage::Graphics) == ImageUsage::Graphics)
					m_vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;

				if ((usage & ImageUsage::Storage) == ImageUsage::Storage)
					m_vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;

				if ((usage & ImageUsage::Color) == ImageUsage::Color)
					m_vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				if ((usage & ImageUsage::Depth) == ImageUsage::Depth)
					m_vFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

				return m_vFlags;
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
				VkComponentMapping m_vComponentMapping = {};
				switch (format)
				{
				case Flint::PixelFormat::Undefined:
				case Flint::PixelFormat::D16_SINT:
				case Flint::PixelFormat::D32_SFLOAT:
				case Flint::PixelFormat::S8_UINT:
				case Flint::PixelFormat::D16_UNORMAL_S8_UINT:
				case Flint::PixelFormat::D24_UNORMAL_S8_UINT:
				case Flint::PixelFormat::D32_SFLOAT_S8_UINT:
					m_vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
					m_vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
					m_vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
					m_vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
					break;

				case Flint::PixelFormat::R8_SRGB:
				case Flint::PixelFormat::R8_UNORMAL:
				case Flint::PixelFormat::R16_SFLOAT:
				case Flint::PixelFormat::R32_SFLOAT:
					m_vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					m_vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					m_vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					m_vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					break;

				case Flint::PixelFormat::R8G8_SRGB:
				case Flint::PixelFormat::R8G8_UNORMAL:
				case Flint::PixelFormat::R16G16_SFLOAT:
				case Flint::PixelFormat::R32G32_SFLOAT:
					m_vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					m_vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
					m_vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ZERO;
					m_vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ZERO;
					break;

				case Flint::PixelFormat::R8G8B8_SRGB:
				case Flint::PixelFormat::R8G8B8_UNORMAL:
				case Flint::PixelFormat::R16G16B16_SFLOAT:
				case Flint::PixelFormat::R32G32B32_SFLOAT:
					m_vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					m_vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
					m_vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
					m_vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ZERO;
					break;

				case Flint::PixelFormat::R8G8B8A8_SRGB:
				case Flint::PixelFormat::R8G8B8A8_UNORMAL:
				case Flint::PixelFormat::R16G16B16A16_SFLOAT:
				case Flint::PixelFormat::R32G32B32A32_SFLOAT:
					m_vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					m_vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
					m_vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
					m_vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;
					break;

				case Flint::PixelFormat::B8G8R8_SRGB:
				case Flint::PixelFormat::B8G8R8_UNORMAL:
					m_vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					m_vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
					m_vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
					m_vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ZERO;
					break;

				case Flint::PixelFormat::B8G8R8A8_SRGB:
				case Flint::PixelFormat::B8G8R8A8_UNORMAL:
					m_vComponentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
					m_vComponentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
					m_vComponentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
					m_vComponentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;
					break;
				}

				return m_vComponentMapping;
			}
		}

		VulkanImage::VulkanImage(VulkanDevice* m_pDevice, const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const uint8_t layers, const uint32_t mipLevels, const void* pImageData, const MultiSampleCount sampleCount)
			: Image(m_pDevice, type, usage, extent, format, layers, mipLevels, pImageData, sampleCount)
		{
			Initialize(pImageData);
		}

		void VulkanImage::GenerateMipMaps()
		{
			OPTICK_EVENT();

			if (m_MipLevels == 1)
				return;

			VkFormatProperties m_vProperties = {};
			vkGetPhysicalDeviceFormatProperties(m_pDevice->GetPhysicalDevice(), Utilities::GetVulkanFormat(m_Format), &m_vProperties);

			if (!(m_vProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
				throw backend_error("Texture format does not support linear bitting!");

			if (m_Type == ImageType::CubeMap || m_Type == ImageType::CubeMapArray)
				GenerateCubeMapMipChain();
			else
				GenerateDefaultMipChain();

			m_vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		std::unique_ptr<VulkanBuffer> VulkanImage::CopyToBuffer()
		{
			std::unique_ptr<VulkanBuffer> pBuffer = std::make_unique<VulkanBuffer>(m_pDevice, BufferType::Staging, static_cast<uint64_t>(m_Extent.m_Width) * m_Extent.m_Height * m_Extent.m_Depth * Utilities::GetByteDepth(m_Format) * m_LayerCount);

			VkBufferImageCopy m_vCopy = {};
			m_vCopy.bufferOffset = 0;
			m_vCopy.bufferImageHeight = m_Extent.m_Height;
			m_vCopy.bufferRowLength = m_Extent.m_Width;
			m_vCopy.imageOffset = { 0, 0, 0 };
			m_vCopy.imageExtent.width = m_Extent.m_Width;
			m_vCopy.imageExtent.height = m_Extent.m_Height;
			m_vCopy.imageExtent.depth = m_Extent.m_Depth;
			m_vCopy.imageSubresource.aspectMask = Utilities::GetImageAspectFlags(m_Usage);
			m_vCopy.imageSubresource.baseArrayLayer = 0;
			m_vCopy.imageSubresource.layerCount = m_LayerCount;
			m_vCopy.imageSubresource.mipLevel = 0;	// TODO

			{
				VulkanOneTimeCommandBuffer m_vCommandBuffer(*m_pDevice);
				VkFormat m_vFormat = Utilities::GetVulkanFormat(m_Format);

				m_pDevice->SetImageLayout(m_vCommandBuffer, m_vImage, m_vCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_vFormat, m_LayerCount, 0, m_MipLevels);
				m_vCurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

				m_pDevice->GetDeviceTable().vkCmdCopyImageToBuffer(m_vCommandBuffer, m_vImage, m_vCurrentLayout, pBuffer->GetBuffer(), 1, &m_vCopy);

				m_pDevice->SetImageLayout(m_vCommandBuffer, m_vImage, m_vCurrentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, m_vFormat, m_LayerCount, 0, m_MipLevels);
				m_vCurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			return pBuffer;
		}

		std::unique_ptr<VulkanImageView> VulkanImage::CreateImageView(const uint32_t baseLayerIndex, const uint32_t layerCount, const uint32_t baseMipLevel, const uint32_t mipLevels, const ImageUsage usage)
		{
			return std::make_unique<VulkanImageView>(m_pDevice, this, baseLayerIndex, layerCount, baseMipLevel, mipLevels, usage);
		}

		void VulkanImage::Terminate()
		{
			m_pDevice->GetDeviceTable().vkDestroyImageView(m_pDevice->GetLogicalDevice(), m_vImageView, nullptr);
			vmaDestroyImage(m_pDevice->GetVmaAllocator(), m_vImage, vmaAllocation);

			bIsTerminated = true;
		}

		void VulkanImage::CopyFromImage(VkCommandBuffer m_vCommandBuffer, VkImage m_vSrcImage, VkImageLayout m_vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers)
		{
			VkImageCopy m_vImageCopy = {};
			m_vImageCopy.extent.width = m_Extent.m_Width;
			m_vImageCopy.extent.height = m_Extent.m_Height;
			m_vImageCopy.extent.depth = m_Extent.m_Depth;
			m_vImageCopy.srcOffset = srcOffset;
			m_vImageCopy.dstOffset = dstOffset;
			m_vImageCopy.srcSubresource = subresourceLayers;
			m_vImageCopy.dstSubresource.layerCount = m_LayerCount;
			m_vImageCopy.dstSubresource.aspectMask = GetAspectFlags();
			m_vImageCopy.dstSubresource.baseArrayLayer = 0;
			m_vImageCopy.dstSubresource.mipLevel = 0; // TODO

			m_pDevice->GetDeviceTable().vkCmdCopyImage(m_vCommandBuffer, m_vSrcImage, m_vSrcLayout, m_vImage, m_vCurrentLayout, 1, &m_vImageCopy);
		}

		void VulkanImage::CopyFromImage(VkImage m_vSrcImage, VkImageLayout m_vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers)
		{
			VulkanOneTimeCommandBuffer m_vCommandBuffer{ *m_pDevice };
			CopyFromImage(m_vCommandBuffer, m_vSrcImage, m_vSrcLayout, srcOffset, dstOffset, subresourceLayers);
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
				return m_vCurrentLayout;

			if ((usage & m_Usage) != usage)
				throw backend_error("Invalid image usage requested!");

			if (usage == ImageUsage::Graphics && m_vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				m_pDevice->SetImageLayout(m_vImage, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels, 0, GetAspectFlags());
				m_vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else if (usage == ImageUsage::Storage && m_vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_GENERAL)
			{
				m_pDevice->SetImageLayout(m_vImage, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels, 0, GetAspectFlags());
				m_vCurrentLayout = VK_IMAGE_LAYOUT_GENERAL;
			}

			return m_vCurrentLayout;
		}

		VkAttachmentDescription VulkanImage::GetAttachmentDescription() const
		{
			VkAttachmentDescription m_vDesc = {};
			m_vDesc.flags = 0;
			m_vDesc.format = GetImageFormat();
			m_vDesc.samples = Utilities::GetSampleCount(m_MultiSampleCount);
			m_vDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			if ((m_Usage & ImageUsage::Graphics) == ImageUsage::Graphics)
				m_vDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			else if ((m_Usage & ImageUsage::Depth) == ImageUsage::Depth)
				m_vDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

			else if ((m_Usage & ImageUsage::Color) == ImageUsage::Color)
				m_vDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			else
				m_vDesc.finalLayout = m_vCurrentLayout;

			m_vDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
			m_vDesc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
			m_vDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			m_vDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;

			if (m_ImageRenderingTargetSpecification == ImageRenderTargetSpecification(0))
				return m_vDesc;

			// Resolve load operation.
			if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::LoadOnLoad)
				m_vDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::ClearOnLoad)
				m_vDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::DiscardLoad)
				m_vDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			// Resolve store operation.
			if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StoreOnStore)
				m_vDesc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::DiscardStore)
				m_vDesc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;

			// Resolve stencil load operation.
			if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StencilLoadOnLoad)
				m_vDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StencilClearOnLoad)
				m_vDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StencilDiscardLoad)
				m_vDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			// Resolve stencil store operation.
			if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StencilStoreOnStore)
				m_vDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;

			else if (m_ImageRenderingTargetSpecification & ImageRenderTargetSpecification::StencilDiscardStore)
				m_vDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;

			return m_vDesc;
		}

		VkImageLayout VulkanImage::GetAttachmentLayout() const
		{
			if ((m_Usage & ImageUsage::Depth) == ImageUsage::Depth)
				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			if ((m_Usage & ImageUsage::Color) == ImageUsage::Color)
				return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			return m_vCurrentLayout;
		}

		VkImageView VulkanImage::GetImageView(uint32_t index) const
		{
			return m_vImageView;
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
			VkImageView m_vImageView = VK_NULL_HANDLE;

			if (m_Type == ImageType::CubeMap || m_Type == ImageType::CubeMapArray)
				m_vImageView = Utilities::CreateImageViews({ m_vImage }, Utilities::GetVulkanFormat(m_Format), m_pDevice->StaticCast<VulkanDevice>(), Utilities::GetImageAspectFlags(m_Usage), Helpers::GetImageViewType(m_Type), 1, layerNumber, m_MipLevels, 0, { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A })[0];
			else
				m_vImageView = Utilities::CreateImageViews({ m_vImage }, Utilities::GetVulkanFormat(m_Format), m_pDevice->StaticCast<VulkanDevice>(), Utilities::GetImageAspectFlags(m_Usage), Helpers::GetImageViewType(m_Type), 1, layerNumber, m_MipLevels, 0)[0];

			return m_vImageView;
		}

		void VulkanImage::SetImageLayout(VkCommandBuffer m_vCommandBuffer, VkImageLayout m_vNewLayout, uint32_t layerCount, uint32_t layerIndex, const uint32_t mipLevels) const
		{
			m_pDevice->SetImageLayout(m_vCommandBuffer, m_vImage, m_vCurrentLayout, m_vNewLayout, Utilities::GetVulkanFormat(m_Format), layerCount, layerIndex, mipLevels);
			m_vCurrentLayout = m_vNewLayout;
		}

		void VulkanImage::SetImageLayout(VkImageLayout m_vNewLayout) const
		{
			VulkanOneTimeCommandBuffer m_vCommandBuffer(*m_pDevice);
			SetImageLayoutManual(m_vCommandBuffer, m_vNewLayout);
		}

		void VulkanImage::SetImageLayoutManual(VkCommandBuffer m_vCommandBuffer, VkImageLayout m_vNewLayout) const
		{
			SetImageLayout(m_vCommandBuffer, m_vNewLayout, m_LayerCount, 0, m_MipLevels);
			m_vCurrentLayout = m_vNewLayout;
		}

		void VulkanImage::CreateImage()
		{
			OPTICK_EVENT();

			m_vCurrentLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;

			VkImageCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.extent.width = m_Extent.m_Width;
			m_vCreateInfo.extent.height = m_Extent.m_Height;
			m_vCreateInfo.extent.depth = m_Extent.m_Depth;
			m_vCreateInfo.arrayLayers = m_LayerCount;
			m_vCreateInfo.format = Utilities::GetVulkanFormat(m_Format);
			m_vCreateInfo.imageType = Helpers::GetImageType(m_Type);
			m_vCreateInfo.initialLayout = m_vCurrentLayout;
			m_vCreateInfo.mipLevels = m_MipLevels;
			m_vCreateInfo.queueFamilyIndexCount = 0;
			m_vCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
			m_vCreateInfo.samples = Utilities::GetSampleCount(m_MultiSampleCount);
			m_vCreateInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
			m_vCreateInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
			m_vCreateInfo.usage = Helpers::GetImageUsage(m_Usage);

			if (m_Type == ImageType::CubeMap || m_Type == ImageType::CubeMapArray)
				m_vCreateInfo.flags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

			if ((m_Usage & ImageUsage::Color) == ImageUsage::Color)
			{
				VkFormatProperties m_vProperties = {};
				vkGetPhysicalDeviceFormatProperties(m_pDevice->GetPhysicalDevice(), Utilities::GetVulkanFormat(m_Format), &m_vProperties);

				if (!(m_vProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT))
					throw backend_error("Texture format does not support using as storage image!");
			}

			// Prepare the queue indexes if the device if the compute and graphics queues are different.
			std::vector<uint32_t> queueIndexes = {};
			if (m_pDevice->IsGraphicsCompatible() && m_pDevice->IsComputeCompatible())
			{
				if (m_pDevice->GetQueue().m_GraphicsFamily != m_pDevice->GetQueue().m_ComputeFamily)
				{
					queueIndexes.emplace_back(m_pDevice->GetQueue().m_GraphicsFamily.value());
					queueIndexes.emplace_back(m_pDevice->GetQueue().m_ComputeFamily.value());

					m_vCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
				}
			}

			m_vCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueIndexes.size());
			m_vCreateInfo.pQueueFamilyIndices = queueIndexes.data();

			//FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreateImage(pDevice->GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vImage));

			VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
			vmaAllocationCreateInfo.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
			vmaCreateImage(m_pDevice->GetVmaAllocator(), &m_vCreateInfo, &vmaAllocationCreateInfo, &m_vImage, &vmaAllocation, nullptr);
		}

		void VulkanImage::CreateImageView()
		{
			OPTICK_EVENT();

			if (m_Type == ImageType::CubeMap || m_Type == ImageType::CubeMapArray || m_Usage == ImageUsage::Storage)
				m_vImageView = Utilities::CreateImageViews({ m_vImage }, Utilities::GetVulkanFormat(m_Format), m_pDevice->StaticCast<VulkanDevice>(), Utilities::GetImageAspectFlags(m_Usage), Helpers::GetImageViewType(m_Type), m_LayerCount, 0, m_MipLevels, 0, Helpers::GetComponentMapping(m_Format))[0];
			else
				m_vImageView = Utilities::CreateImageViews({ m_vImage }, Utilities::GetVulkanFormat(m_Format), m_pDevice->StaticCast<VulkanDevice>(), Utilities::GetImageAspectFlags(m_Usage), Helpers::GetImageViewType(m_Type), m_LayerCount, 0, m_MipLevels, 0)[0];
		}

		void VulkanImage::GenerateDefaultMipChain()
		{
			VulkanOneTimeCommandBuffer m_vCommandBuffer(*m_pDevice);
			if (m_vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				m_pDevice->SetImageLayout(m_vCommandBuffer, m_vImage, m_vCurrentLayout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				m_vCurrentLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			}

			for (uint32_t i = 0; i < m_LayerCount; i++)
			{
				VkImageMemoryBarrier barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.image = m_vImage;
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

					m_pDevice->GetDeviceTable().vkCmdPipelineBarrier(m_vCommandBuffer,
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

					m_pDevice->GetDeviceTable().vkCmdBlitImage(m_vCommandBuffer,
						m_vImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						m_vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1, &blit,
						VK_FILTER_LINEAR);

					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

					m_pDevice->GetDeviceTable().vkCmdPipelineBarrier(m_vCommandBuffer,
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

				m_pDevice->GetDeviceTable().vkCmdPipelineBarrier(m_vCommandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);
			}
		}

		void VulkanImage::GenerateCubeMapMipChain()
		{
			VkFormatProperties m_vProperties = {};
			vkGetPhysicalDeviceFormatProperties(m_pDevice->GetPhysicalDevice(), Utilities::GetVulkanFormat(m_Format), &m_vProperties);

			VulkanOneTimeCommandBuffer m_vCommandBuffer(*m_pDevice);
			if (m_vCurrentLayout != VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				m_pDevice->SetImageLayout(m_vCommandBuffer, m_vImage, m_vCurrentLayout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				m_vCurrentLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			}

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = m_vImage;
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

				m_pDevice->GetDeviceTable().vkCmdPipelineBarrier(m_vCommandBuffer,
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

				m_pDevice->GetDeviceTable().vkCmdBlitImage(m_vCommandBuffer,
					m_vImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					m_vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				m_pDevice->GetDeviceTable().vkCmdPipelineBarrier(m_vCommandBuffer,
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

			m_pDevice->GetDeviceTable().vkCmdPipelineBarrier(m_vCommandBuffer,
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
				VulkanBuffer m_vStagingBuffer(m_pDevice, BufferType::Staging, size);

				std::byte* pBytes = static_cast<std::byte*>(m_vStagingBuffer.MapMemory(size));
				std::copy(static_cast<const std::byte*>(pImageData), static_cast<const std::byte*>(pImageData) + size, pBytes);
				m_vStagingBuffer.UnmapMemory();

				m_pDevice->SetImageLayout(m_vImage, m_vCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				m_vCurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

				// Copy from buffer. TODO
				{
					VkBufferImageCopy m_vCopy = {};
					m_vCopy.bufferImageHeight = 0;
					m_vCopy.bufferRowLength = 0;
					m_vCopy.bufferOffset = 0;
					m_vCopy.imageOffset = { 0, 0, 0 };
					m_vCopy.imageExtent.width = m_Extent.m_Width;
					m_vCopy.imageExtent.height = m_Extent.m_Height;
					m_vCopy.imageExtent.depth = m_Extent.m_Depth;
					m_vCopy.imageSubresource.aspectMask = Utilities::GetImageAspectFlags(m_Usage);
					m_vCopy.imageSubresource.baseArrayLayer = 0;
					m_vCopy.imageSubresource.layerCount = m_LayerCount;
					m_vCopy.imageSubresource.mipLevel = 0;

					VulkanOneTimeCommandBuffer m_vCommandBuffer(*m_pDevice);
					m_pDevice->GetDeviceTable().vkCmdCopyBufferToImage(m_vCommandBuffer, m_vStagingBuffer.GetBuffer(), m_vImage, m_vCurrentLayout, 1, &m_vCopy);
				}

				m_vStagingBuffer.Terminate();
			}
			if ((m_Usage & ImageUsage::Depth) == ImageUsage::Depth)
			{
				m_vCurrentLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;	// TODO
			}
			else if ((m_Usage & ImageUsage::Graphics) == ImageUsage::Graphics)
			{
				m_pDevice->SetImageLayout(m_vImage, m_vCurrentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				m_vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else if ((m_Usage & ImageUsage::Color) == ImageUsage::Color)
			{
				m_pDevice->SetImageLayout(m_vImage, m_vCurrentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				m_vCurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			else if ((m_Usage & ImageUsage::Storage) == ImageUsage::Storage)
			{
				m_pDevice->SetImageLayout(m_vImage, m_vCurrentLayout, VK_IMAGE_LAYOUT_GENERAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				m_vCurrentLayout = VK_IMAGE_LAYOUT_GENERAL;
			}
			else
			{
				m_pDevice->SetImageLayout(m_vImage, m_vCurrentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Utilities::GetVulkanFormat(m_Format), m_LayerCount, 0, m_MipLevels);
				m_vCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
		}
	}
}