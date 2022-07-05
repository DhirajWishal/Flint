// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanDispatcher.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		void Dispatch(PFN_vkDestroyInstance vkDestroyInstance, Synchronized<VkInstance>& instance, VkAllocationCallbacks* pAllocator)
		{
			instance.apply([vkDestroyInstance, pAllocator](VkInstance& instance) { vkDestroyInstance(instance, pAllocator); });
		}

		void Dispatch(PFN_vkDestroyDevice vkDestroyDevice, Synchronized<VkDevice>& device, VkAllocationCallbacks* pAllocator)
		{
			device.apply([vkDestroyDevice, pAllocator](VkDevice& device) { vkDestroyDevice(device, pAllocator); });
		}
	}
}