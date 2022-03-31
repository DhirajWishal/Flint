// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanHostSynchronizationPrimitive.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanHostSynchronizationPrimitive::VulkanHostSynchronizationPrimitive(VulkanDevice* m_pDevice)
			: HostSynchronizationPrimitive(m_pDevice)
		{
			OPTICK_EVENT();

			VkFenceCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkCreateFence(m_pDevice->GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vFence));
		}

		void VulkanHostSynchronizationPrimitive::Wait(const uint64_t timeout)
		{
			OPTICK_EVENT();
			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkWaitForFences(m_pDevice->GetLogicalDevice(), 1, &m_vFence, VK_TRUE, timeout));
		}

		void VulkanHostSynchronizationPrimitive::Reset()
		{
			OPTICK_EVENT();
			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkResetFences(m_pDevice->GetLogicalDevice(), 1, &m_vFence));
		}

		void VulkanHostSynchronizationPrimitive::Terminate()
		{
			m_pDevice->GetDeviceTable().vkDestroyFence(m_pDevice->GetLogicalDevice(), m_vFence, nullptr);
			bIsTerminated = true;
		}
	}
}