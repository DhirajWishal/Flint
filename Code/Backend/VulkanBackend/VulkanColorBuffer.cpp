// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanColorBuffer.hpp"
#include "VulkanUtilities.hpp"
#include "VulkanOneTimeCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanColorBuffer::VulkanColorBuffer(VulkanDevice& device, const FExtent2D& extent, const UI32 bufferCount, VkFormat format)
			: VulkanRenderTargetAttachment(RenderTargetAttachmenType::COLOR_BUFFER, device, extent, bufferCount, format)
		{
			vSampleCount = vDevice.GetSampleCount();

			Initialize();
		}

		void VulkanColorBuffer::Recreate(const FExtent2D& extent)
		{
			mExtent = extent;

			Terminate();
			Initialize();
		}

		void VulkanColorBuffer::Terminate()
		{
			for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
				vkDestroyImage(vDevice.GetLogicalDevice(), *itr, nullptr);

			vkFreeMemory(vDevice.GetLogicalDevice(), vBufferMemory, nullptr);

			for (auto itr = vImageViews.begin(); itr != vImageViews.end(); itr++)
				vkDestroyImageView(vDevice.GetLogicalDevice(), *itr, nullptr);
		}

		VkAttachmentDescription VulkanColorBuffer::GetAttachmentDescription() const
		{
			VkAttachmentDescription vDesc = {};
			vDesc.flags = 0;
			vDesc.format = vFormat;
			vDesc.samples = static_cast<VkSampleCountFlagBits>(vSampleCount);
			vDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			vDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			vDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			vDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			vDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			return vDesc;
		}

		VkImageLayout VulkanColorBuffer::GetAttachmentLayout() const
		{
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		void VulkanColorBuffer::Initialize()
		{
			VkImageCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			vCI.flags = 0;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.imageType = VK_IMAGE_TYPE_2D;
			vCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			vCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			vCI.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			vCI.extent = { static_cast<UI32>(mExtent.mWidth), static_cast<UI32>(mExtent.mHeight), 1 };
			vCI.samples = static_cast<VkSampleCountFlagBits>(vSampleCount);
			vCI.format = vFormat;
			vCI.arrayLayers = 1;
			vCI.mipLevels = 1;
			vCI.queueFamilyIndexCount = 0;
			vCI.pQueueFamilyIndices = VK_NULL_HANDLE;

			vImages.resize(mBufferCount);
			for (UI32 i = 0; i < mBufferCount; i++)
				FLINT_VK_ASSERT(vkCreateImage(vDevice.GetLogicalDevice(), &vCI, nullptr, vImages.data() + i));

			FLINT_VK_ASSERT(vDevice.CreateImageMemory(vImages, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vBufferMemory));
			vImageViews = std::move(Utilities::CreateImageViews(vImages, vFormat, vDevice, VK_IMAGE_ASPECT_COLOR_BIT));

			{
				VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);
				for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
					vDevice.SetImageLayout(vCommandBuffer, *itr, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, vFormat);
			}
		}
	}
}