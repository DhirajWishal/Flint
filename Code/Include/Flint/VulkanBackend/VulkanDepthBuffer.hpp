// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDepthBuffer final : public VulkanRenderTargetAttachment
		{
		public:
			VulkanDepthBuffer(VulkanDevice &device, const FExtent2D &extent, const UI32 bufferCount)
				: VulkanRenderTargetAttachment(RenderTargetAttachmenType::DEPTH_BUFFER, device, extent, bufferCount)
			{
				vSampleCount = vDevice.GetSampleCount();
				vFormat = Utilities::FindDepthFormat(vDevice.GetPhysicalDevice());

				Initialize();
			}

			virtual void Recreate(const FExtent2D &extent) override final
			{
				mExtent = extent;

				Terminate();
				Initialize();
			}

			virtual void Terminate() override final
			{
				for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
					vkDestroyImage(vDevice.GetLogicalDevice(), *itr, nullptr);

				vkFreeMemory(vDevice.GetLogicalDevice(), vBufferMemory, nullptr);

				for (auto itr = vImageViews.begin(); itr != vImageViews.end(); itr++)
					vkDestroyImageView(vDevice.GetLogicalDevice(), *itr, nullptr);
			}

			virtual VkAttachmentDescription GetAttachmentDescription() const override final
			{
				VkAttachmentDescription vDesc = {};
				vDesc.format = vFormat;
				vDesc.samples = static_cast<VkSampleCountFlagBits>(vSampleCount);
				vDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				vDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				vDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				vDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				vDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				vDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				return vDesc;
			}

			virtual VkImageLayout GetAttachmentLayout() const override final
			{
				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}

		private:
			void Initialize()
			{
				VkImageCreateInfo vCI = {};
				vCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				vCI.flags = 0;
				vCI.pNext = VK_NULL_HANDLE;
				vCI.extent = {static_cast<UI32>(mExtent.mWidth), static_cast<UI32>(mExtent.mHeight), 1};
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

				vImages.resize(mBufferCount);
				for (UI32 i = 0; i < mBufferCount; i++)
					FLINT_VK_ASSERT(vkCreateImage(vDevice.GetLogicalDevice(), &vCI, nullptr, vImages.data() + i));

				FLINT_VK_ASSERT(vDevice.CreateImageMemory(vImages, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vBufferMemory));
				vImageViews = std::move(Utilities::CreateImageViews(vImages, vFormat, vDevice, VK_IMAGE_ASPECT_DEPTH_BIT));

				{
					VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);
					for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
						vDevice.SetImageLayout(vCommandBuffer, *itr, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, vFormat);
				}
			}

		private:
			VkDeviceMemory vBufferMemory = VK_NULL_HANDLE;
			VkSampleCountFlags vSampleCount = VK_SAMPLE_COUNT_64_BIT;
		};
	}
}
