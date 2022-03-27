// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanHostSynchronizationPrimitive.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanHostSynchronizationPrimitive::VulkanHostSynchronizationPrimitive(VulkanDevice* pDevice)
			: HostSynchronizationPrimitive(pDevice)
		{
			OPTICK_EVENT();

			VkFenceCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreateFence(pDevice->GetLogicalDevice(), &vCreateInfo, nullptr, &vFence));
		}

		void VulkanHostSynchronizationPrimitive::Wait(const uint64_t timeout)
		{
			OPTICK_EVENT();
			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkWaitForFences(pDevice->GetLogicalDevice(), 1, &vFence, VK_TRUE, timeout));
		}

		void VulkanHostSynchronizationPrimitive::Reset()
		{
			OPTICK_EVENT();
			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkResetFences(pDevice->GetLogicalDevice(), 1, &vFence));
		}

		void VulkanHostSynchronizationPrimitive::Terminate()
		{
			pDevice->GetDeviceTable().vkDestroyFence(pDevice->GetLogicalDevice(), vFence, nullptr);
			bIsTerminated = true;
		}
	}
}