// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/SynchronizationPrimitive.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanSynchronizationPrimitive final : public SynchronizationPrimitive
		{
		public:
			VulkanSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice);
			~VulkanSynchronizationPrimitive() { if (!bIsTerminated) Terminate(); }

			virtual void Wait(const UI64 timeout = std::numeric_limits<UI64>::max()) override;
			virtual void Reset() override;
			virtual void Terminate() override;

			const VkFence GetFence() const { return vFence; }

		private:
			VkFence vFence = VK_NULL_HANDLE;
		};
	}
}