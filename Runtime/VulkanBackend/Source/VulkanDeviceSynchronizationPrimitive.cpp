// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanDeviceSynchronizationPrimitive.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanDeviceSynchronizationPrimitive::VulkanDeviceSynchronizationPrimitive(Device* pDevice)
			: DeviceSynchronizationPrimitive(pDevice)
		{
			VkSemaphoreCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateSemaphore(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vSemaphore));
		}

		void VulkanDeviceSynchronizationPrimitive::Terminate()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkDestroySemaphore(vDevice.GetLogicalDevice(), vSemaphore, nullptr);

			bIsTerminated = true;
		}
	}
}