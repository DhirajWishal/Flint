// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Vulkan image class.
		 */
		class VulkanImage final
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device reference.
			 */
			explicit VulkanImage(const std::shared_ptr<VulkanDevice>& pDevice);

		private:
			VulkanDevice& m_Device;

			VkImage m_Image = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;
		};
	}
}