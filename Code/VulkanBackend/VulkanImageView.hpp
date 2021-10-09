// Copyright 2021 Dhiraj Wishal
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
			VulkanImageView(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Image>& pImage, const UI32 baseLayerIndex, const UI32 layerCount, const UI32 baseMipLevel, const UI32 mipLevels, const ImageUsage usage);
			~VulkanImageView() { if (!bIsTerminated) Terminate(); }

			virtual void Terminate() override;

			const VkImageView GetImageView() const { return vImageView; }

		private:
			VkImageView vImageView = VK_NULL_HANDLE;
		};
	}
}
