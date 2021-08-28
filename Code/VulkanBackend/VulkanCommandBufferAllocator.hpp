// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/CommandBufferAllocator.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferAllocator final : public CommandBufferAllocator, public std::enable_shared_from_this<VulkanCommandBufferAllocator>
		{
		public:
			VulkanCommandBufferAllocator(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount);
			VulkanCommandBufferAllocator(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<CommandBufferAllocator>& pParent, const UI32 bufferCount);
			~VulkanCommandBufferAllocator() { if(!bIsTerminated) Terminate(); }

			virtual const std::vector<std::shared_ptr<CommandBuffer>> CreateCommandBuffers() override final;
			virtual std::shared_ptr<CommandBufferAllocator> CreateChildAllocator() override final;
			virtual void Terminate() override final;

		private:
			VkCommandPool vCommandPool = VK_NULL_HANDLE;
		};
	}
}