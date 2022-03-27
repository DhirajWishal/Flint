// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ImageSampler.hpp"
#include "VulkanBackend/VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanImageSampler final : public ImageSampler<VulkanDevice>
		{
		public:
			VulkanImageSampler(VulkanDevice* pDevice, const ImageSamplerSpecification& specification);
			~VulkanImageSampler() { if (!bIsTerminated) Terminate(); }

			virtual void Terminate() override;

			const VkSampler GetSampler() const { return vSampler; }

		private:
			VkSampler vSampler = VK_NULL_HANDLE;
		};
	}
}