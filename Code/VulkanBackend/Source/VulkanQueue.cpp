// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanQueue.hpp"

#include <vector>

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanQueue::Initialize(VkPhysicalDevice vPhysicalDevice, const DeviceFlags flags)
		{
			FLINT_SETUP_PROFILER();

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
					if (itr->queueFlags & VK_QUEUE_GRAPHICS_BIT && (flags & DeviceFlags::GraphicsCompatible) == DeviceFlags::GraphicsCompatible)
						mGraphicsFamily = i;

					// Set compute family.
					if (itr->queueFlags & VK_QUEUE_COMPUTE_BIT && (flags & DeviceFlags::ComputeCompatible) == DeviceFlags::ComputeCompatible)
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

		bool VulkanQueue::IsComplete() const
		{
			return (mGraphicsFamily.has_value() || mComputeFamily.has_value()) && mTransferFamily.has_value();
		}
	}
}