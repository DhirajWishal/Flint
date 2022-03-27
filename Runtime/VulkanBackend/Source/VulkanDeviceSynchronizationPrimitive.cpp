// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanDeviceSynchronizationPrimitive.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanDeviceSynchronizationPrimitive::VulkanDeviceSynchronizationPrimitive(VulkanDevice* pDevice)
			: DeviceSynchronizationPrimitive(pDevice)
		{
			VkSemaphoreCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;

			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreateSemaphore(pDevice->GetLogicalDevice(), &vCreateInfo, nullptr, &vSemaphore));
		}

		void VulkanDeviceSynchronizationPrimitive::Terminate()
		{
			pDevice->GetDeviceTable().vkDestroySemaphore(pDevice->GetLogicalDevice(), vSemaphore, nullptr);

			bIsTerminated = true;
		}
	}
}