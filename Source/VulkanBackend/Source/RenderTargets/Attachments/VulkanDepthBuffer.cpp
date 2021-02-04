// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/Attachments/VulkanDepthBuffer.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/VulkanUtilities.h"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanDepthBuffer::Initialize(VulkanDevice* pDevice, const Vector2& extent, UI32 bufferCount)
		{
			this->pDevice = pDevice;
			this->mExtent = extent;
			this->mBufferCount = bufferCount;
			this->vFormat = Utilities::FindDepthFormat(pDevice->GetPhysicalDevice());
			vSampleCount = pDevice->GetSampleCount();

			VkImageCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.extent = { static_cast<UI32>(extent.x), static_cast<UI32>(extent.y), 1 };
			vCI.samples = static_cast<VkSampleCountFlagBits>(vSampleCount);
			vCI.format = vFormat;
			vCI.arrayLayers = 1;
			vCI.mipLevels = 1;
			vCI.queueFamilyIndexCount = 0;
			vCI.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCI.imageType = VK_IMAGE_TYPE_2D;
			vCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			vCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			vCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

			vImages.resize(bufferCount);
			for (UI32 i = 0; i < bufferCount; i++)
				FLINT_VK_ASSERT(pDevice->CreateImage(&vCI, vImages.data() + i), "Failed to create Vulkan Image!")

				FLINT_VK_ASSERT(pDevice->CreateImageMemory(vImages, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vBufferMemory), "Failed to bind image memory!");
			vImageViews = std::move(Utilities::CreateImageViews(vImages, vFormat, pDevice, VK_IMAGE_ASPECT_DEPTH_BIT));

			{
				VulkanOneTimeCommandBuffer vCommandBuffer(pDevice);
				for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
					pDevice->SetImageLayout(vCommandBuffer, *itr, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, vFormat);
			}
		}

		void VulkanDepthBuffer::Terminate()
		{
			for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
				pDevice->DestroyImage(*itr);

			pDevice->FreeMemory(vBufferMemory);

			for (auto itr = vImageViews.begin(); itr != vImageViews.end(); itr++)
				pDevice->DestroyImageView(*itr);
		}

		VkAttachmentDescription VulkanDepthBuffer::GetAttachmentDescription() const
		{
			VkAttachmentDescription vDesc = {};
			vDesc.format = vFormat;
			vDesc.samples = static_cast<VkSampleCountFlagBits>(vSampleCount);
			vDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			vDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			vDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			vDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			vDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			return vDesc;
		}

		VkImageLayout VulkanDepthBuffer::GetAttachmentLayout() const
		{
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
	}
}