// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include <volk.h>
export module Flint.VulkanBackend.VulkanHostSynchronizationPrimitive;

#include "GraphicsCore/HostSynchronizationPrimitive.hpp"
import Flint.VulkanBackend.VulkanDevice;

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanHostSynchronizationPrimitive final : public HostSynchronizationPrimitive
		{
		public:
			VulkanHostSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice);
			~VulkanHostSynchronizationPrimitive() { if (!bIsTerminated) Terminate(); }

			virtual void Wait(const uint64 timeout = std::numeric_limits<uint64>::max()) override;
			virtual void Reset() override;
			virtual void Terminate() override;

			const VkFence GetFence() const { return vFence; }

		private:
			VkFence vFence = VK_NULL_HANDLE;
		};
	}
}

module: private;

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanHostSynchronizationPrimitive::VulkanHostSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice)
			: HostSynchronizationPrimitive(pDevice)
		{
			OPTICK_EVENT();

			VkFenceCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateFence(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vFence));
		}

		void VulkanHostSynchronizationPrimitive::Wait(const uint64 timeout)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkWaitForFences(vDevice.GetLogicalDevice(), 1, &vFence, VK_TRUE, timeout));
		}

		void VulkanHostSynchronizationPrimitive::Reset()
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkResetFences(vDevice.GetLogicalDevice(), 1, &vFence));
		}

		void VulkanHostSynchronizationPrimitive::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkDestroyFence(vDevice.GetLogicalDevice(), vFence, nullptr);

			bIsTerminated = true;
		}
	}
}