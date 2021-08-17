// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Image.hpp"
#include "VulkanRenderTargetAttachment.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanImage final : public Image, public VulkanRenderTargetAttachmentInterface, public std::enable_shared_from_this<VulkanImage>
		{
		public:
			VulkanImage(const std::shared_ptr<Device>& pDevice, ImageType type, ImageUsage usage, const FBox3D& extent, PixelFormat format, UI8 layers, UI32 mipLevels, const void* pImageData);

			virtual std::shared_ptr<Buffer> CopyToBuffer() override final;
			virtual void Terminate() override final;

			void CopyFromImage(VkImage vSrcImage, VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers);

		public:
			virtual void Recreate(const FBox2D& extent) override final;

			virtual VkImageLayout GetImageLayout() const { return vCurrentLayout; }
			virtual VkAttachmentDescription GetAttachmentDescription() const override final;
			virtual VkImageLayout GetAttachmentLayout() const override final;
			virtual VkImageView GetImageView(UI32 index) const override final;
			virtual RenderTargetAttachmenType GetAttachmentType() const override final;
			virtual VkFormat GetImageFormat() const override final;

		public:
			VkImageView CreateLayerBasedImageView(UI32 layerNumber) const;

		private:
			void CreateImage();
			void CreateImageMemory();
			void CreateImageView();
			void GenerateMipMaps();

		private:
			VkImage vImage = VK_NULL_HANDLE;
			VkDeviceMemory vImageMemory = VK_NULL_HANDLE;
			VkImageView vImageView = VK_NULL_HANDLE;

			VkImageLayout vCurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		};
	}
}