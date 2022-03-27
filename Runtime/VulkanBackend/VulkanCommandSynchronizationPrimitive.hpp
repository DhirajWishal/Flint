// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/CommandSynchronizationPrimitive.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandSynchronizationPrimitive final : public CommandSynchronizationPrimitive
		{
		public:
			VulkanCommandSynchronizationPrimitive(Device* pDevice);
			~VulkanCommandSynchronizationPrimitive() { if (!bIsTerminated) Terminate(); }

			virtual void Terminate() override;
		};
	}
}