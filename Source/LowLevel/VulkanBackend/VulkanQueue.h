// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"
#include <optional>

#include "MetaLoader/volk.h" 

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanQueue {
		public:
			VulkanQueue() {}
			~VulkanQueue() {}

			void Initialize(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface);
			bool IsComplete() const;

			std::optional<UI32> mGraphicsFamily;
			std::optional<UI32> mComputeFamily;
			std::optional<UI32> mTransferFamily;

			VkQueue vGraphicsQueue = VK_NULL_HANDLE;
			VkQueue vComputeQueue = VK_NULL_HANDLE;
			VkQueue vTransferQueue = VK_NULL_HANDLE;
		};
	}
}