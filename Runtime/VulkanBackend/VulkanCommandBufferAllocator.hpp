// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/CommandBufferAllocator.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBuffer;

		class VulkanCommandBufferAllocator final : public CommandBufferAllocator<VulkanCommandBufferAllocator, VulkanDevice, VulkanCommandBuffer>
		{
		public:
			VulkanCommandBufferAllocator(VulkanDevice* pDevice, const uint32_t bufferCount);
			VulkanCommandBufferAllocator(VulkanDevice* pDevice, VulkanCommandBufferAllocator* pParent, const uint32_t bufferCount);
			~VulkanCommandBufferAllocator() { if(!bIsTerminated) Terminate(); }

			virtual const std::vector<std::shared_ptr<VulkanCommandBuffer>> CreateCommandBuffers() override;
			virtual std::unique_ptr<VulkanCommandBufferAllocator> CreateChildAllocator() override;
			virtual void Terminate() override;

		private:
			VkCommandPool vCommandPool = VK_NULL_HANDLE;
		};
	}
}