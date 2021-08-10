// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

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

		class VulkanRenderTargetAttachmentInterface
		{
		public:
			VulkanRenderTargetAttachmentInterface() = default;

			virtual void Recreate(const FBox2D& extent) = 0;

			virtual VkAttachmentDescription GetAttachmentDescription() const = 0;
			virtual VkImageLayout GetAttachmentLayout() const = 0;
			virtual VkImageView GetImageView(UI32 index) const = 0;
			virtual RenderTargetAttachmenType GetAttachmentType() const = 0;
			virtual VkFormat GetImageFormat() const = 0;
		};

		class VulkanRenderTargetAttachment : public VulkanRenderTargetAttachmentInterface
		{
		public:
			VulkanRenderTargetAttachment(RenderTargetAttachmenType type, VulkanDevice& device, const FBox2D& extent, const UI32 bufferCount, VkFormat format)
				: mType(type), vDevice(device), mExtent(extent), mBufferCount(bufferCount), vFormat(format) {}

			VulkanRenderTargetAttachment(RenderTargetAttachmenType type, VulkanDevice& device, const FBox2D& extent, const UI32 bufferCount)
				: mType(type), vDevice(device), mExtent(extent), mBufferCount(bufferCount) {}

			virtual VkImageView GetImageView(UI32 index) const override final { return vImageViews[index]; }

		public:
			virtual RenderTargetAttachmenType GetAttachmentType() const override final { return mType; }
			virtual VkFormat GetImageFormat() const override final { return vFormat; }

		protected:
			std::vector<VkImage> vImages;
			std::vector<VkImageView> vImageViews;
			VulkanDevice& vDevice;
			FBox2D mExtent = {};
			UI32 mBufferCount = 0;
			VkFormat vFormat = VkFormat::VK_FORMAT_UNDEFINED;
			RenderTargetAttachmenType mType = RenderTargetAttachmenType::UNDEFINED;
		};
	}
}