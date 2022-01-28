// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include <volk.h>
export module Flint.VulkanBackend.VulkanCommandSynchronizationPrimitive;

#include "GraphicsCore/CommandSynchronizationPrimitive.hpp"
import Flint.VulkanBackend.VulkanDevice;

export namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandSynchronizationPrimitive final : public CommandSynchronizationPrimitive
		{
		public:
			VulkanCommandSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice);
			~VulkanCommandSynchronizationPrimitive() { if (!bIsTerminated) Terminate(); }

			virtual void Terminate() override;
		};
	}
}

module: private;

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandSynchronizationPrimitive::VulkanCommandSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice)
			: CommandSynchronizationPrimitive(pDevice)
		{
		}

		void VulkanCommandSynchronizationPrimitive::Terminate()
		{
		}
	}
}