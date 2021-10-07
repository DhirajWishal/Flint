// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Image.hpp"
#include "VulkanRenderTargetAttachmentInterface.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanImage final : public Image, public VulkanRenderTargetAttachmentInterface, public std::enable_shared_from_this<VulkanImage>
		{
		public:
			VulkanImage(
				const std::shared_ptr<Device>& pDevice,
				const ImageType type,
				const ImageUsage usage,
				const FBox3D& extent,
				const PixelFormat format,
				const UI8 layers,
				const UI32 mipLevels,
				const void* pImageData,
				const MultiSampleCount sampleCount = MultiSampleCount::One);
			~VulkanImage() { if (!bIsTerminated) Terminate(); }

			virtual void GenerateMipMaps() override;
			virtual std::shared_ptr<Buffer> CopyToBuffer() override;
			virtual void Terminate() override;

			void CopyFromImage(VkImage vSrcImage, VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers);

		public:
			virtual void Recreate(const FBox2D& extent);

			virtual VkImageLayout GetImageLayout(ImageUsage usage = ImageUsage(-1)) const;
			virtual VkAttachmentDescription GetAttachmentDescription() const override;
			virtual VkImageLayout GetAttachmentLayout() const override;
			virtual VkImageView GetImageView(UI32 index) const override;
			virtual RenderTargetAttachmenType GetAttachmentType() const override;
			virtual VkFormat GetImageFormat() const override;

		public:
			VkImageView CreateLayerBasedImageView(UI32 layerNumber) const;
			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImageLayout vNewLayout, UI32 layerCount = 1, UI32 layerIndex = 0, const UI32 mipLevels = 1) const;
			void SetImageLayout(VkImageLayout vNewLayout) const;

			const VkImage GetImage() const { return vImage; }

		private:
			void CreateImage();
			void CreateImageView();

			void GenerateDefaultMipChain();
			void GenerateCubeMapMipChain();

			void Initialize(const void* pImageData);

		private:
			VkImage vImage = VK_NULL_HANDLE;
			VkImageView vImageView = VK_NULL_HANDLE;

			VmaAllocation vmaAllocation = {};

			mutable VkImageLayout vCurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		};
	}
}