// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanDeviceSynchronizationPrimitive.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanDeviceSynchronizationPrimitive::VulkanDeviceSynchronizationPrimitive(VulkanDevice* m_pDevice)
			: DeviceSynchronizationPrimitive(m_pDevice)
		{
			VkSemaphoreCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;

			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkCreateSemaphore(m_pDevice->GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vSemaphore));
		}

		void VulkanDeviceSynchronizationPrimitive::Terminate()
		{
			m_pDevice->GetDeviceTable().vkDestroySemaphore(m_pDevice->GetLogicalDevice(), m_vSemaphore, nullptr);

			bIsTerminated = true;
		}
	}
}