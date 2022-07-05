// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Containers/Synchronized.hpp"
#include "Core/Errors/BackendError.hpp"

#include <vulkan/vulkan.h>

namespace Flint
{
	namespace VulkanBackend
	{
		void Dispatch(PFN_vkDestroyInstance vkDestroyInstance, Synchronized<VkInstance>& instance, VkAllocationCallbacks* pAllocator);
		void Dispatch(PFN_vkDestroyDevice vkDestroyDevice, Synchronized<VkDevice>& device, VkAllocationCallbacks* pAllocator);
	}
}