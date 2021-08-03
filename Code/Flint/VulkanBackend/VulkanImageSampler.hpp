// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ImageSampler.hpp"
#include "VulkanBackend/VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanImageSampler final : public ImageSampler
		{
		public:
			VulkanImageSampler(const std::shared_ptr<Device>& pDevice, const ImageSamplerSpecification& specification);

			virtual void Terminate() override final;

			const VkSampler GetSampler() const { return vSampler; }

		private:
			VkSampler vSampler = VK_NULL_HANDLE;
		};
	}
}