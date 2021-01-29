// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanResourceContainer final : public VulkanDeviceBoundObject {
		public:
			VulkanResourceContainer() {}
			~VulkanResourceContainer() {}

			void Initialize(VulkanDevice* pDevice, const std::vector<VkDescriptorPoolSize>& sizes);
			virtual void Terminate() override final;

		public:
			VkDescriptorPool vPool = VK_NULL_HANDLE;
			VkDescriptorSet vSet = VK_NULL_HANDLE;
		};
	}
}