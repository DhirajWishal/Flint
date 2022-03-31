// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanOneTimeCommandBuffer {
		public:
			explicit VulkanOneTimeCommandBuffer(const VulkanDevice& device);
			~VulkanOneTimeCommandBuffer();

			operator VkCommandBuffer() const { return m_vBuffer; }
			operator VkCommandPool() const { return m_vPool; }

			VkCommandBuffer GetBuffer() const { return m_vBuffer; }
			VkCommandPool GetPool() const { return m_vPool; }

		private:
			VulkanDevice& m_vDevice;

			VkCommandBuffer m_vBuffer = VK_NULL_HANDLE;
			VkCommandPool m_vPool = VK_NULL_HANDLE;
		};
	}
}