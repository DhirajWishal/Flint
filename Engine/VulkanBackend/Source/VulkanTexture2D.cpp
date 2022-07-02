// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanTexture2D.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanCommandBuffers.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanTexture2D::VulkanTexture2D(const std::shared_ptr<VulkanDevice>& pDevice, uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format, uint32_t mipLevels /*= 0*/, Multisample multisampleCount /*= Multisample::One*/, const std::byte* pDataStore /*= nullptr*/)
			: Texture2D(pDevice, width, height, usage, format, mipLevels, multisampleCount, pDataStore)
		{
			// Create the image and allocator.
			createImageAndAllocator();

			// Copy data if provided.
			if (pDataStore)
				copyFrom(pDataStore);
		}

		VulkanTexture2D::~VulkanTexture2D()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanTexture2D::terminate()
		{
			vmaDestroyImage(getDevice().as<VulkanDevice>()->getAllocator(), m_vImage, m_Allocation);
			invalidate();
		}

		std::shared_ptr<Flint::Buffer> VulkanTexture2D::toBuffer() const
		{
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
			auto commandBuffers = VulkanCommandBuffers(getDevicePointerAs<VulkanDevice>());
			commandBuffers.begin();

			copyFromBatched(commandBuffers, pBuffer);

			commandBuffers.end();
			commandBuffers.submitTransfer();
			commandBuffers.finishExecution();
		}

		void VulkanTexture2D::copyFrom(const std::byte* pDataStore)
		{
			auto pBuffer = getDevicePointerAs<VulkanDevice>()->createBuffer(static_cast<uint64_t>(m_Width) * m_Height * GetPixelSize(m_Format), BufferUsage::Staging);
			copyFrom(pBuffer.get());
		}

		void VulkanTexture2D::copyFrom(const Texture2D* pTexutre)
		{
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
			VkImageSubresourceLayers subresource = {};
			subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresource.baseArrayLayer = 0;
			subresource.layerCount = 1;
			subresource.mipLevel = 0;

			auto pBuffer = getDevicePointerAs<VulkanDevice>()->createBuffer(static_cast<uint64_t>(m_Width) * m_Height * GetPixelSize(m_Format), BufferUsage::Staging);
			commandBuffers.changeImageLayout(m_vImage, m_CurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
			commandBuffers.copyImageToBuffer(m_vImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, { m_Width, m_Height, 1 }, { 0, 0, 0 }, subresource, pBuffer->as<VulkanBuffer>()->getBuffer(), 0, m_Height, m_Width);
			commandBuffers.changeImageLayout(m_vImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_GENERAL : m_CurrentLayout, subresource.aspectMask);

			return pBuffer;
		}

		void VulkanTexture2D::copyFromBatched(const VulkanCommandBuffers& commandBuffers, const Buffer* pBuffer)
		{
			VkImageSubresourceLayers subresource = {};
			subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresource.baseArrayLayer = 0;
			subresource.layerCount = 1;
			subresource.mipLevel = 0;

			commandBuffers.changeImageLayout(m_vImage, m_CurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
			commandBuffers.copyBufferToImage(pBuffer->as<VulkanBuffer>()->getBuffer(), 0, m_Height, m_Width, m_vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, { m_Width, m_Height, 1 }, { 0, 0, 0 }, subresource);
			commandBuffers.changeImageLayout(m_vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_GENERAL : m_CurrentLayout, subresource.aspectMask);
		}

		void VulkanTexture2D::createImageAndAllocator()
		{
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

			FLINT_VK_ASSERT(vmaCreateImage(getDevice().as<VulkanDevice>()->getAllocator(), &createInfo, &allocationCreateInfo, &m_vImage, &m_Allocation, nullptr), "Failed to create the image!");

			// Make sure to validate!
			validate();
		}
	}
}