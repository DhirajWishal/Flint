// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBufferAllocator.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferAllocator final : public CommandBufferAllocator, public std::enable_shared_from_this<VulkanCommandBufferAllocator>
		{
		public:
			VulkanCommandBufferAllocator(const std::shared_ptr<Device>& pDevice, UI32 bufferCount);
			VulkanCommandBufferAllocator(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<CommandBufferAllocator>& pParent, UI32 bufferCount);
			~VulkanCommandBufferAllocator() { if(!bIsTerminated) Terminate(); }

			virtual void Terminate() override final;

		private:
			VkCommandPool vCommandPool = VK_NULL_HANDLE;
		};
	}
}