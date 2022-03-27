// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Image.hpp"
#include "VulkanRenderTargetAttachmentInterface.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanImageView;

		class VulkanImage final : public Image<VulkanDevice, VulkanBuffer, VulkanImageView>, public VulkanRenderTargetAttachmentInterface
		{
		public:
			VulkanImage(VulkanDevice* pDevice, const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const uint8_t layers, const uint32_t mipLevels, const void* pImageData, const MultiSampleCount sampleCount = MultiSampleCount::One);
			~VulkanImage() { if (!bIsTerminated) Terminate(); }

			virtual void GenerateMipMaps() override;
			virtual std::unique_ptr<VulkanBuffer> CopyToBuffer() override;
			virtual std::unique_ptr<VulkanImageView> CreateImageView(const uint32_t baseLayerIndex, const uint32_t layerCount, const uint32_t baseMipLevel, const uint32_t mipLevels, const ImageUsage usage) override;
			virtual void Terminate() override;

			void CopyFromImage(VkCommandBuffer vCommandBuffer, VkImage vSrcImage, VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers);
			void CopyFromImage(VkImage vSrcImage, VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers);

		public:
			virtual void Recreate(const FBox2D& extent);

			virtual VkImageLayout GetImageLayout(ImageUsage usage = ImageUsage(-1)) const;
			virtual VkAttachmentDescription GetAttachmentDescription() const override;
			virtual VkImageLayout GetAttachmentLayout() const override;
			virtual VkImageView GetImageView(uint32_t index) const override;
			virtual RenderTargetAttachmenType GetAttachmentType() const override;
			virtual VkFormat GetImageFormat() const override;

			VkImageViewType GetImageViewType() const;
			VkImageAspectFlags GetAspectFlags() const;
			VkComponentMapping GetComponentMapping() const;

		public:
			VkImageView CreateLayerBasedImageView(uint32_t layerNumber) const;
			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImageLayout vNewLayout, uint32_t layerCount = 1, uint32_t layerIndex = 0, const uint32_t mipLevels = 1) const;
			void SetImageLayout(VkImageLayout vNewLayout) const;
			void SetImageLayoutManual(VkCommandBuffer vCommandBuffer, VkImageLayout vNewLayout) const;

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