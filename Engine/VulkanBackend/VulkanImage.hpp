// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanEngine.hpp"

namespace Flint
{
	namespace VulkanBackend
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
			 * @param engine The engine reference.
			 */
			explicit VulkanImage(VulkanEngine& engine);

		private:
			VulkanEngine& m_Engine;

			VkImage m_Image = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;
		};
	}
}