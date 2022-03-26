// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/HostSynchronizationPrimitive.hpp"
#include "VulkanDevice.hpp"

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