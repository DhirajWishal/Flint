// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBackend/VulkanDevice.h"
#include "Flint/FRenderTargetManager.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRenderTargetAttachment : public VulkanDeviceBoundObject
		{
		public:
			VulkanRenderTargetAttachment(VulkanDevice* pDevice, const RenderTargetAttachmentType& type, const FExtent2D& extent, UI32 bufferCount)
				: VulkanDeviceBoundObject(pDevice), mType(type), mExtent(extent), mBufferCount(bufferCount) {}
			VulkanRenderTargetAttachment(VulkanDevice* pDevice, const RenderTargetAttachmentType& type, const FExtent2D& extent, UI32 bufferCount, VkFormat format)
				: VulkanDeviceBoundObject(pDevice),  mType(type), mExtent(extent), mBufferCount(bufferCount), vFormat(format) {}

			virtual void Recreate(const FExtent2D& extent) {}

			virtual VkAttachmentDescription GetAttachmentDescription() const { return VkAttachmentDescription(); }
			virtual VkImageLayout GetAttachmentLayout() const { return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED; }

			VkImageView GetImageView(UI32 index) const { return vImageViews[index]; }

		public:
			RenderTargetAttachmentType GetType() const { return mType; }
			VkFormat GetFormat() const { return vFormat; }

		protected:
			std::vector<VkImage> vImages;
			std::vector<VkImageView> vImageViews;

			FExtent2D mExtent = {};
			UI32 mBufferCount = 0;

			VkFormat vFormat = VkFormat::VK_FORMAT_UNDEFINED;
			RenderTargetAttachmentType mType = RenderTargetAttachmentType::SWAP_CHAIN;
		};
	}
}