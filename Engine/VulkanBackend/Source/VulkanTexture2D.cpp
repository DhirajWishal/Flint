// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanTexture2D.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanCommandBuffers.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanTextureView.hpp"

#include <Optick.h>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanTexture2D::VulkanTexture2D(const std::shared_ptr<VulkanDevice>& pDevice, uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format, uint32_t mipLevels /*= 0*/, Multisample multisampleCount /*= Multisample::One*/, const std::byte* pDataStore /*= nullptr*/)
			: Texture2D(pDevice, width, height, usage, format, mipLevels, multisampleCount, pDataStore)
		{
			OPTICK_EVENT();

			// Create the image and allocator.
			createImageAndAllocator();

			// Copy data if provided.
			if (pDataStore)
				copyFrom(pDataStore);

			// Generate the mipmaps if required.
			if (m_MipLevels > 1)
				generateMipMaps();

			// Else transfer the layout to shader read only if usage is graphics.
			else if (usage == ImageUsage::Graphics)
			{
				auto commandBuffers = VulkanCommandBuffers(pDevice);
				commandBuffers.begin();

				commandBuffers.changeImageLayout(m_Image, m_CurrentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
				m_CurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				commandBuffers.end();
				commandBuffers.submitTransfer();
				commandBuffers.finishExecution();
			}

			// Else if we need the image for storage, let's prepare it.
			else if (usage == ImageUsage::Storage)
			{
				auto commandBuffers = VulkanCommandBuffers(pDevice);
				commandBuffers.begin();

				commandBuffers.changeImageLayout(m_Image, m_CurrentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
				m_CurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				commandBuffers.end();
				commandBuffers.submitTransfer();
				commandBuffers.finishExecution();
			}
		}

		VulkanTexture2D::~VulkanTexture2D()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanTexture2D::terminate()
		{
			OPTICK_EVENT();

			getDevice().as<VulkanDevice>()->getAllocator().apply([this](VmaAllocator& allocator) { vmaDestroyImage(allocator, m_Image, m_Allocation); });
			invalidate();
		}

		std::shared_ptr<Flint::TextureView> VulkanTexture2D::createView()
		{
			OPTICK_EVENT();

			return std::make_shared<VulkanTextureView>(getDevicePointerAs<VulkanDevice>(), shared_from_this());
		}

		std::shared_ptr<Flint::Buffer> VulkanTexture2D::toBuffer() const
		{
			OPTICK_EVENT();

			auto commandBuffers = VulkanCommandBuffers(getDevicePointerAs<VulkanDevice>());
			commandBuffers.begin();

			auto pBuffer = toBufferBatched(commandBuffers);

			commandBuffers.end();
			commandBuffers.submitTransfer();
			commandBuffers.finishExecution();

			return pBuffer;
		}

		void VulkanTexture2D::copyFrom(const Buffer* pBuffer)
		{
			OPTICK_EVENT();

			auto commandBuffers = VulkanCommandBuffers(getDevicePointerAs<VulkanDevice>());
			commandBuffers.begin();

			copyFromBatched(commandBuffers, pBuffer);

			commandBuffers.end();
			commandBuffers.submitTransfer();
			commandBuffers.finishExecution();
		}

		void VulkanTexture2D::copyFrom(const std::byte* pDataStore)
		{
			OPTICK_EVENT();

			auto pBuffer = getDevicePointerAs<VulkanDevice>()->createBuffer(static_cast<uint64_t>(m_Width) * m_Height * GetPixelSize(m_Format), BufferUsage::Staging, pDataStore);
			copyFrom(pBuffer.get());
		}

		void VulkanTexture2D::copyFrom(const Texture* pTexutre)
		{
			OPTICK_EVENT();

			auto commandBuffers = VulkanCommandBuffers(getDevicePointerAs<VulkanDevice>());
			commandBuffers.begin();

			auto pBuffer = pTexutre->as<VulkanTexture2D>()->toBufferBatched(commandBuffers);
			copyFromBatched(commandBuffers, pBuffer.get());

			commandBuffers.end();
			commandBuffers.submitTransfer();
			commandBuffers.finishExecution();
		}


		std::shared_ptr<Flint::Buffer> VulkanTexture2D::toBufferBatched(const VulkanCommandBuffers& commandBuffers) const
		{
			OPTICK_EVENT();

			VkImageSubresourceLayers subresource = {};
			subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresource.baseArrayLayer = 0;
			subresource.layerCount = 1;
			subresource.mipLevel = 0;

			auto pBuffer = getDevicePointerAs<VulkanDevice>()->createBuffer(static_cast<uint64_t>(m_Width) * m_Height * GetPixelSize(m_Format), BufferUsage::Staging);
			commandBuffers.changeImageLayout(m_Image, m_CurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
			commandBuffers.copyImageToBuffer(m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, { m_Width, m_Height, 1 }, { 0, 0, 0 }, subresource, pBuffer->as<VulkanBuffer>()->getBuffer(), 0, m_Height, m_Width);
			commandBuffers.changeImageLayout(m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_GENERAL : m_CurrentLayout, subresource.aspectMask, m_MipLevels);

			return pBuffer;
		}

		void VulkanTexture2D::copyFromBatched(const VulkanCommandBuffers& commandBuffers, const Buffer* pBuffer)
		{
			OPTICK_EVENT();

			VkImageSubresourceLayers subresource = {};
			subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresource.baseArrayLayer = 0;
			subresource.layerCount = 1;
			subresource.mipLevel = 0;

			commandBuffers.changeImageLayout(m_Image, m_CurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
			commandBuffers.copyBufferToImage(pBuffer->as<VulkanBuffer>()->getBuffer(), 0, m_Height, m_Width, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, { m_Width, m_Height, 1 }, { 0, 0, 0 }, subresource);
			commandBuffers.changeImageLayout(m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_GENERAL : m_CurrentLayout, subresource.aspectMask, m_MipLevels);
		}

		void VulkanTexture2D::generateMipMaps()
		{
			OPTICK_EVENT();

			auto pVulkanDevice = getDevicePointerAs<VulkanDevice>();
			auto commandBuffers = VulkanCommandBuffers(pVulkanDevice);
			commandBuffers.begin();

			if (m_CurrentLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				commandBuffers.changeImageLayout(m_Image, m_CurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
				m_CurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			}

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = m_Image;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;

			int32_t mipWidth = static_cast<int32_t>(m_Width);
			int32_t mipHeight = static_cast<int32_t>(m_Height);
			int32_t mipDepth = 1;

			for (uint32_t j = 1; j < m_MipLevels; j++)
			{
				barrier.subresourceRange.baseMipLevel = j - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				pVulkanDevice->getDeviceTable().vkCmdPipelineBarrier(commandBuffers.getCurrentBuffer(),
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
				blit.srcSubresource.layerCount = 1;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, mipDepth > 1 ? mipDepth / 2 : 1 };
				blit.dstSubresource.aspectMask = barrier.subresourceRange.aspectMask;
				blit.dstSubresource.mipLevel = j;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = 1;

				pVulkanDevice->getDeviceTable().vkCmdBlitImage(commandBuffers.getCurrentBuffer(),
					m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				pVulkanDevice->getDeviceTable().vkCmdPipelineBarrier(commandBuffers.getCurrentBuffer(),
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

			pVulkanDevice->getDeviceTable().vkCmdPipelineBarrier(commandBuffers.getCurrentBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			commandBuffers.end();
			commandBuffers.submitTransfer();
			commandBuffers.finishExecution();
		}

		void VulkanTexture2D::createImageAndAllocator()
		{
			OPTICK_EVENT();

			// Resolve the image usage.
			VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

			if ((m_Usage & ImageUsage::Graphics) == ImageUsage::Graphics)
				usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

			if ((m_Usage & ImageUsage::Storage) == ImageUsage::Storage)
				usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;

			// Create the image.
			VkImageCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;
			createInfo.extent.width = m_Width;
			createInfo.extent.height = m_Height;
			createInfo.extent.depth = 1;
			createInfo.format = Utility::GetImageFormat(m_Format);
			createInfo.arrayLayers = 1;
			createInfo.initialLayout = m_CurrentLayout;
			createInfo.imageType = VK_IMAGE_TYPE_2D;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
			createInfo.samples = Utility::GetSampleCountFlagBits(m_MultisampleCount);
			createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			createInfo.usage = usageFlags;
			createInfo.mipLevels = m_MipLevels;

			VmaAllocationCreateInfo allocationCreateInfo = {};
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

			getDevice().as<VulkanDevice>()->getAllocator().apply([this, createInfo, allocationCreateInfo](VmaAllocator& allocator)
				{
					FLINT_VK_ASSERT(vmaCreateImage(allocator, &createInfo, &allocationCreateInfo, &m_Image, &m_Allocation, nullptr), "Failed to create the image!");
				}
			);

			// Make sure to validate!
			validate();
		}
	}
}