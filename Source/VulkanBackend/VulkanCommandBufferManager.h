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
			~VulkanCommandBufferManager() {}

			virtual void CreateBuffers(Backend::Device* pDevice, UI32 count) override final;
			virtual void Terminate() override final;

			virtual void BeginBufferRecording(UI32 index) override final;
			virtual void EndBufferRecording(UI32 index) override final;

			virtual I32 BeginCommandExecution(UI32 frameIndex, Backend::ScreenBoundRenderTarget* pRenderTarget) override final;
			virtual void SubmitCommand(UI32 index, Backend::ScreenBoundRenderTarget* pRenderTarget) override final;

			virtual void DrawUsingIndexData(UI32 index, UI32 indexCount, UI32 vertexOffset, UI32 indexOffset) override final;

			VkCommandPool vCommandPool = VK_NULL_HANDLE;
			std::vector<VkCommandBuffer> vCommandBuffers;

		private:
			void InitializeSyncObjects(UI32 count);
			void TerminateSyncObjects();

		private:
			std::vector<VkSemaphore> vImageAvailables;
			std::vector<VkSemaphore> vRenderFinishes;
			std::vector<VkFence> vInFlightFences;
		};
	}
}