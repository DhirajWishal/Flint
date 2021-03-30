// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\Image.h"
#include "VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanImage final : public Backend::Image<VulkanDevice> {
		public:
			using DeviceType = VulkanDevice;

		public:
			VulkanImage() {}
			~VulkanImage() {}

			virtual void Initialize(DeviceType* pDevice, UI64 width, UI64 height, UI64 depth, Backend::ImageUsage usage, UI8 bitsPerPixel = FLINT_DEFAULT_BPP, UI8 layers = 1) override final;
			virtual void Terminate() override final;

			virtual void CopyData(unsigned char* pData) override final;

			void SetImageLayout(VkImageLayout layout);

		private:
			void GenerateMipMaps();

			VkImage vImage = VK_NULL_HANDLE;
			VkImageView vImageView = VK_NULL_HANDLE;
			VkDeviceMemory vImageMemory = VK_NULL_HANDLE;

			VkFormat vFormat = VkFormat::VK_FORMAT_UNDEFINED;
			VkImageLayout vCurrentLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		};
	}
}