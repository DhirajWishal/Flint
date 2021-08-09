// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Image.hpp"

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanImage final : public Image, public std::enable_shared_from_this<VulkanImage>
		{
		public:
			VulkanImage(const std::shared_ptr<Device>& pDevice, ImageType type, ImageUsage usage, const FBox3D& extent, PixelFormat format, UI8 layers, UI32 mipLevels, const void* pImageData);

			virtual void Terminate() override final;

			void CopyFromImage(VkImage vSrcImage, VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, VkImageSubresourceLayers subresourceLayers);

			const VkImageView GetImageView() const { return vImageView; }
			const VkImageLayout GetImageLayout() const { return vCurrentLayout; }

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