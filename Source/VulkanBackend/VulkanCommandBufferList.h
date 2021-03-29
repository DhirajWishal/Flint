// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\CommandBufferList.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferList final : public Backend::CommandBufferList<VulkanCommandBufferList, VulkanDevice, VulkanBuffer> {
		public:
			VulkanCommandBufferList() {}
			~VulkanCommandBufferList() {}

			virtual void mInitialize() override final;
			virtual void mTerminate() override final;

			virtual void mBeginBuffer() override final;
			virtual void mEndBuffer() override final;

			virtual void mBindBuffer(const BufferType& buffer) override final;
			virtual void mBindVertexBuffer(const BufferType& buffer, UI64 firstBinding, UI64 bindingCount) override final;

			VkCommandPool GetCommandPool() const { return vCommandPool; }
			VkCommandBuffer GetCurrentBuffer() const { return vBuffers[GetIndex()]; }

		private:
			VkCommandPool vCommandPool = VK_NULL_HANDLE;
			std::vector<VkCommandBuffer> vBuffers = {};
		};
	}
}