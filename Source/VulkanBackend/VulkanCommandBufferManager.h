// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/CommandBufferManager.h"
#include "VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferManager final : public Backend::CommandBufferManager {
		public:
			VulkanCommandBufferManager() {}
			VulkanCommandBufferManager(Backend::Device* pDevice, VkCommandPool pool, std::vector<VkCommandBuffer>&& buffers)
				: vCommandPool(pool), vCommandBuffers(std::move(buffers)), Backend::CommandBufferManager(pDevice) {}
			VulkanCommandBufferManager(Backend::Device* pDevice, VkCommandPool pool, std::vector<VkCommandBuffer>&& buffers, std::vector<std::shared_ptr<Backend::CommandBuffer>>&& commandBuffers)
				: vCommandPool(pool), vCommandBuffers(std::move(buffers)), Backend::CommandBufferManager(pDevice, std::move(commandBuffers)) {}
			~VulkanCommandBufferManager() {}

			virtual void CreateBuffers(Backend::Device* pDevice, UI32 count) override final;
			virtual void Terminate() override final;

			virtual std::shared_ptr<Backend::CommandBufferManager> CreateChild(UI32 bufferCount) override final;
			virtual void UpdateChild(Backend::CommandBufferManager* pChildBufferManager, Backend::RenderTarget* pRenderTarget) override final;

			virtual void ClearBuffers() override final;
			virtual void RecreateBuffers() override final;

			VkCommandPool vCommandPool = VK_NULL_HANDLE;
			std::vector<VkCommandBuffer> vCommandBuffers;
			VkCommandBufferInheritanceInfo vCBII = {};
		};
	}
}