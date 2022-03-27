// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandSynchronizationPrimitive.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandSynchronizationPrimitive::VulkanCommandSynchronizationPrimitive(Device* pDevice)
			: CommandSynchronizationPrimitive(pDevice)
		{
		}

		void VulkanCommandSynchronizationPrimitive::Terminate()
		{
		}
	}
}