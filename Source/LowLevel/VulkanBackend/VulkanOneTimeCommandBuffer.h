// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanOneTimeCommandBuffer {
		public:
			VulkanOneTimeCommandBuffer(const VulkanDevice* pDevice);
			~VulkanOneTimeCommandBuffer();

			operator VkCommandBuffer() const { return vBuffer; }
			operator VkCommandPool() const { return vPool; }

			VkCommandBuffer GetBuffer() const { return vBuffer; }
			VkCommandPool GetPool() const { return vPool; }

		private:
			VulkanDevice* pDevice = nullptr;

			VkCommandBuffer vBuffer = VK_NULL_HANDLE;
			VkCommandPool vPool = VK_NULL_HANDLE;
		};
	}
}