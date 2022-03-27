// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/CommandBufferAllocator.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferAllocator final : public CommandBufferAllocator
		{
		public:
			VulkanCommandBufferAllocator(Device* pDevice, const uint32_t bufferCount);
			VulkanCommandBufferAllocator(Device* pDevice, CommandBufferAllocator* pParent, const uint32_t bufferCount);
			~VulkanCommandBufferAllocator() { if(!bIsTerminated) Terminate(); }

			virtual const std::vector<std::shared_ptr<CommandBuffer>> CreateCommandBuffers() override;
			virtual std::unique_ptr<CommandBufferAllocator> CreateChildAllocator() override;
			virtual void Terminate() override;

		private:
			VkCommandPool vCommandPool = VK_NULL_HANDLE;
		};
	}
}