// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/HostSynchronizationPrimitive.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanHostSynchronizationPrimitive final : public HostSynchronizationPrimitive<VulkanDevice>
		{
		public:
			explicit VulkanHostSynchronizationPrimitive(VulkanDevice* pDevice);
			~VulkanHostSynchronizationPrimitive() { if (!bIsTerminated) Terminate(); }

			virtual void Wait(const uint64_t timeout = std::numeric_limits<uint64_t>::max()) override;
			virtual void Reset() override;
			virtual void Terminate() override;

			const VkFence GetFence() const { return m_vFence; }

		private:
			VkFence m_vFence = VK_NULL_HANDLE;
		};
	}
}