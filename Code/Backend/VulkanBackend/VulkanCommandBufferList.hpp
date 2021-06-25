// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBufferList.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferList final : public CommandBufferList
		{
		public:
			VulkanCommandBufferList(Device& device, const UI32 bufferCount);
			VulkanCommandBufferList(Device& device, const UI32 bufferCount, CommandBufferList& parent);

			virtual void BeginBufferRecording(UI32 index) override final;
			virtual void BeginBufferRecording(UI32 index, CommandBufferList& parent) override final;
			virtual void BeginNextBufferRecording() override final;
			virtual void BeginNextBufferRecording(CommandBufferList& parent) override final;
			virtual void EndBufferRecording() override final;
			virtual void Terminate() override final;

			VkCommandPool GetCommandPool() const { return vCommandPool; }
			VkCommandBuffer GetCurrentCommandBuffer() const { return vCurrentBuffer; }
			VkCommandBuffer GetCommandBuffer(UI32 index) const { return vCommandBuffers[index]; }
			std::vector<VkCommandBuffer> GetCommandBuffers() const { return vCommandBuffers; }

			VkCommandBufferInheritanceInfo GetInheritanceInfo() const;

		private:
			VkCommandPool vCommandPool = VK_NULL_HANDLE;
			std::vector<VkCommandBuffer> vCommandBuffers = {};

			VkCommandBuffer vCurrentBuffer = VK_NULL_HANDLE;
		};
	}
}