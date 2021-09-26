// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanSynchronizationPrimitive.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanSynchronizationPrimitive::VulkanSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice)
			: SynchronizationPrimitive(pDevice)
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			VkFenceCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateFence(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vFence));
		}

		void VulkanSynchronizationPrimitive::Wait(const UI64 timeout)
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkWaitForFences(vDevice.GetLogicalDevice(), 1, &vFence, VK_TRUE, timeout));
		}

		void VulkanSynchronizationPrimitive::Reset()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkResetFences(vDevice.GetLogicalDevice(), 1, &vFence));
		}

		void VulkanSynchronizationPrimitive::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkDestroyFence(vDevice.GetLogicalDevice(), vFence, nullptr);

			bIsTerminated = true;
		}
	}
}