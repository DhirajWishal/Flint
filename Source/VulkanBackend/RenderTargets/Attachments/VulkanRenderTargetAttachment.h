// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBackend\VulkanDevice.h"
#include "Core\Backend\RenderTarget.h"

namespace Flint
{
	namespace VulkanBackend
	{
		enum class RenderTargetAttachmenType : UI8 {
			UNDEFINED,
			SWAP_CHAIN,
			COLOR_BUFFER,
			DEPTH_BUFFER,
		};

		class VulkanRenderTargetAttachment : public VulkanDeviceBoundObject {
		public:
			VulkanRenderTargetAttachment(const RenderTargetAttachmenType& type) : mType(type) {}

			virtual void Initialize(VulkanDevice* pDevice, const Vector2& extent, UI32 bufferCount) {}
			virtual void Initialize(VulkanDevice* pDevice, VulkanDisplay* pDisplay, const Vector2& extent, UI32 bufferCount) {}
			virtual void Initialize(VulkanDevice* pDevice, const Vector2& extent, VkFormat format, UI32 bufferCount) {}

			virtual void Recreate(const Vector2& extent) {}

			virtual VkAttachmentDescription GetAttachmentDescription() const { return VkAttachmentDescription(); }
			virtual VkImageLayout GetAttachmentLayout() const { return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED; }

			VkImageView GetImageView(UI32 index) const { return vImageViews[index]; }

		public:
			RenderTargetAttachmenType GetType() const { return mType; }
			VkFormat GetFormat() const { return vFormat; }

		protected:
			std::vector<VkImage> vImages;
			std::vector<VkImageView> vImageViews;

			Vector2 mExtent = {};
			UI32 mBufferCount = 0;

			VkFormat vFormat = VkFormat::VK_FORMAT_UNDEFINED;
			RenderTargetAttachmenType mType = RenderTargetAttachmenType::UNDEFINED;
		};
	}
}