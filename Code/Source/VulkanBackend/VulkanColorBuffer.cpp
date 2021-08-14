// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanColorBuffer.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanColorBuffer::VulkanColorBuffer(VulkanDevice& device, const FBox2D& extent, const UI32 bufferCount, VkFormat format)
			: VulkanRenderTargetAttachment(RenderTargetAttachmenType::COLOR_BUFFER, device, extent, bufferCount, format)
		{
			FLINT_SETUP_PROFILER();

			vSampleCount = vDevice.GetSampleCount();

			Initialize();
		}

		void VulkanColorBuffer::Recreate(const FBox2D& extent)
		{
			FLINT_SETUP_PROFILER();

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
			FLINT_SETUP_PROFILER();

			VkImageCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			vCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			vCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			vCreateInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			vCreateInfo.extent = { static_cast<UI32>(mExtent.mWidth), static_cast<UI32>(mExtent.mHeight), 1 };
			vCreateInfo.samples = static_cast<VkSampleCountFlagBits>(vSampleCount);
			vCreateInfo.format = vFormat;
			vCreateInfo.arrayLayers = 1;
			vCreateInfo.mipLevels = 1;
			vCreateInfo.queueFamilyIndexCount = 0;
			vCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;

			vImages.resize(mBufferCount);
			for (UI32 i = 0; i < mBufferCount; i++)
				FLINT_VK_ASSERT(vkCreateImage(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, vImages.data() + i));

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