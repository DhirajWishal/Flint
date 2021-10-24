// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandSynchronizationPrimitive.hpp"

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