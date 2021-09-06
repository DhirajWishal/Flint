// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Instance.hpp"

#include <vulkan/vulkan.h>
#include <optional>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanQueue
		{
		public:
			VulkanQueue() = default;

			void Initialize(VkPhysicalDevice vPhysicalDevice, const DeviceFlags flags);
			bool IsComplete() const;

		public:
			std::optional<UI32> mGraphicsFamily;
			std::optional<UI32> mComputeFamily;
			std::optional<UI32> mTransferFamily;

			VkQueue vGraphicsQueue = VK_NULL_HANDLE;
			VkQueue vComputeQueue = VK_NULL_HANDLE;
			VkQueue vTransferQueue = VK_NULL_HANDLE;
		};
	}
}