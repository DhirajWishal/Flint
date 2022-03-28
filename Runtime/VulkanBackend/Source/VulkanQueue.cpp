// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanQueue.hpp"

#include <vector>

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanQueue::Initialize(VkPhysicalDevice vPhysicalDevice, const DeviceFlags flags)
		{
			OPTICK_EVENT();

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &queueFamilyCount, queueFamilies.data());

			int32_t i = 0;
			for (auto itr = queueFamilies.begin(); itr != queueFamilies.end(); itr++, i++)
			{
				if (itr->queueCount > 0)
				{
					// Set graphics family.
					if (itr->queueFlags & VK_QUEUE_GRAPHICS_BIT && (flags & DeviceFlags::GraphicsCompatible) == DeviceFlags::GraphicsCompatible)
						m_GraphicsFamily = i;

					// Set compute family.
					if (itr->queueFlags & VK_QUEUE_COMPUTE_BIT && (flags & DeviceFlags::ComputeCompatible) == DeviceFlags::ComputeCompatible)
						m_ComputeFamily = i;

					// Set transfer family.
					if (itr->queueFlags & VK_QUEUE_TRANSFER_BIT)
						m_TransferFamily = i;

					// Escape from the loop if the queues were found.
					if (IsComplete())
						return;
				}
			}

			throw backend_error("A queue wasn't found with the required flags!");
		}

		bool VulkanQueue::IsComplete() const
		{
			return (m_GraphicsFamily.has_value() || m_ComputeFamily.has_value()) && m_TransferFamily.has_value();
		}
	}
}