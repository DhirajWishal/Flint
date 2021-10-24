// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/DeviceSynchronizationPrimitive.hpp"
#include "VulkanDevice.hpp"

namespace Flint
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