// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include <volk.h>
export module Flint.VulkanBackend.VulkanDeviceSynchronizationPrimitive;

#include "GraphicsCore/DeviceSynchronizationPrimitive.hpp"
import Flint.VulkanBackend.VulkanDevice;

export namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDeviceSynchronizationPrimitive final : public DeviceSynchronizationPrimitive
		{
		public:
			VulkanDeviceSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice);
			~VulkanDeviceSynchronizationPrimitive() { if (!bIsTerminated) Terminate(); }

			virtual void Terminate() override;

			const VkSemaphore GetSemaphore() const { return vSemaphore; }

		private:
			VkSemaphore vSemaphore = VK_NULL_HANDLE;
		};
	}
}

module: private;

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanDeviceSynchronizationPrimitive::VulkanDeviceSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice)
			: DeviceSynchronizationPrimitive(pDevice)
		{
			VkSemaphoreCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			Utilities::CheckResult(vDevice.GetDeviceTable().vkCreateSemaphore(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vSemaphore));
		}

		void VulkanDeviceSynchronizationPrimitive::Terminate()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkDestroySemaphore(vDevice.GetLogicalDevice(), vSemaphore, nullptr);

			bIsTerminated = true;
		}
	}
}