// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/DeviceSynchronizationPrimitive.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDeviceSynchronizationPrimitive final : public DeviceSynchronizationPrimitive<VulkanDevice>
		{
		public:
			explicit VulkanDeviceSynchronizationPrimitive(VulkanDevice* pDevice);
			~VulkanDeviceSynchronizationPrimitive() { if (!bIsTerminated) Terminate(); }

			virtual void Terminate() override;

			const VkSemaphore GetSemaphore() const { return m_vSemaphore; }

		private:
			VkSemaphore m_vSemaphore = VK_NULL_HANDLE;
		};
	}
}