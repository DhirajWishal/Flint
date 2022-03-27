// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ImageView.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanImageView final : public ImageView
		{
		public:
			VulkanImageView(Device* pDevice, const std::shared_ptr<Image>& pImage, const uint32_t baseLayerIndex, const uint32_t layerCount, const uint32_t baseMipLevel, const uint32_t mipLevels, const ImageUsage usage);
			~VulkanImageView() { if (!bIsTerminated) Terminate(); }

			virtual void Terminate() override;

			const VkImageView GetImageView() const { return vImageView; }

		private:
			VkImageView vImageView = VK_NULL_HANDLE;
		};
	}
}
