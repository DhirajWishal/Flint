// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/CommandBuffer.h"
#include <vulkan/vulkan.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBuffer final : public Backend::CommandBuffer {
		public:
			VulkanCommandBuffer() {}
			VulkanCommandBuffer(VkCommandBuffer buffer, VkCommandBufferInheritanceInfo* pInfo = nullptr) : vBuffer(buffer), pInheritanceInfo(pInfo) {}
			~VulkanCommandBuffer() {}

			virtual void BeginBufferRecording() override final;
			virtual void EndBufferRecording() override final;

			virtual void SubmitDynamicStates(const Backend::DynamicStateContainer& container) override final;
			virtual void DrawUsingIndexData(UI32 indexCount, UI32 vertexOffset, UI32 indexOffset) override final;
			virtual void ExecuteChildCommands(const std::vector<std::shared_ptr<Backend::CommandBuffer>>& pCommandBuffers) override final;

			operator VkCommandBuffer() const { return vBuffer; }
			operator VkCommandBuffer* () { return &vBuffer; }
			operator const VkCommandBuffer* () const { return &vBuffer; }

		private:
			VkCommandBuffer vBuffer = VK_NULL_HANDLE;
			VkCommandBufferInheritanceInfo* pInheritanceInfo = nullptr;
		};
	}
}