// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Instance.hpp"

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanQueue
		{
		public:
			VulkanQueue() = default;

			void Initialize(VkPhysicalDevice vPhysicalDevice, DeviceFlags flags)
			{
				UI32 queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &queueFamilyCount, nullptr);

				std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &queueFamilyCount, queueFamilies.data());

				I32 i = 0;
				for (auto itr = queueFamilies.begin(); itr != queueFamilies.end(); itr++, i++)
				{
					if (itr->queueCount > 0)
					{
						// Set graphics family.
						if (itr->queueFlags & VK_QUEUE_GRAPHICS_BIT && (flags & DeviceFlags::GRAPHICS_COMPATIBLE) == DeviceFlags::GRAPHICS_COMPATIBLE)
							mGraphicsFamily = i;

						// Set compute family.
						if (itr->queueFlags & VK_QUEUE_COMPUTE_BIT && (flags & DeviceFlags::COMPUTE_COMPATIBLE) == DeviceFlags::COMPUTE_COMPATIBLE)
							mComputeFamily = i;

						// Set transfer family.
						if (itr->queueFlags & VK_QUEUE_TRANSFER_BIT)
							mTransferFamily = i;

						// Escape from the loop if the queues were found.
						if (IsComplete())
							break;
					}
				}
			}

			bool IsComplete() const
			{
				return mGraphicsFamily.has_value() && mComputeFamily.has_value() && mTransferFamily.has_value();
			}

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