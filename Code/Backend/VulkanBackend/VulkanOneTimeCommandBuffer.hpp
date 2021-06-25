// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanOneTimeCommandBuffer {
		public:
			VulkanOneTimeCommandBuffer(const VulkanDevice& device);
			~VulkanOneTimeCommandBuffer();

			operator VkCommandBuffer() const { return vBuffer; }
			operator VkCommandPool() const { return vPool; }

			VkCommandBuffer GetBuffer() const { return vBuffer; }
			VkCommandPool GetPool() const { return vPool; }

		private:
			VulkanDevice& vDevice;

			VkCommandBuffer vBuffer = VK_NULL_HANDLE;
			VkCommandPool vPool = VK_NULL_HANDLE;
		};
	}
}