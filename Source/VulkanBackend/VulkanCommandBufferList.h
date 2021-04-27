// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\CommandBufferList.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "RenderTargets\Pipelines\VulkanPipeline.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanScreenBoundRenderTargetS;
		class VulkanGraphicsPipeline;

		class VulkanCommandBufferList final : public FCommandBufferList {		
		public:
			VulkanCommandBufferList() {}
			~VulkanCommandBufferList() {}

			virtual void Initialize(FDevice* pDevice, UI64 bufferCount) override final;
			virtual void Terminate() override final;

			virtual void ClearBuffers() override final;
			virtual void ReceateBuffers() override final;

			virtual void BeginBuffer(UI64 index) override final;
			virtual void EndBuffer() override final;

			virtual void BindVertexBuffer(const FBuffer* pBuffer) override final;
			virtual void BindIndexBuffer(const FBuffer* pBuffer) override final;
			virtual void BindRenderTarget(const FScreenBoundRenderTarget* pRenderTarget) override final;
			virtual void EndRenderTarget() override final;
			virtual void BindPipeline(const FGraphicsPipeline* pGraphicsPipeline) override final;
			virtual void SetDynamicStates(const DynamicStateContainer& container) override final;
			virtual void BindRenderResource(const FPipelineResource* pResource) override final;
			virtual void DrawIndexed(UI64 indexCount, UI64 indexOffset, UI64 vertexOffset) override final;

			VkCommandPool GetCommandPool() const { return vCommandPool; }
			VkCommandBuffer GetCurrentBuffer() const { return vBuffers[GetIndex()]; }
			VkCommandBuffer GetVkBuffer(UI64 index) const { return vBuffers[index]; }

		private:
			VkCommandPool vCommandPool = VK_NULL_HANDLE;
			std::vector<VkCommandBuffer> vBuffers = {};
		};
	}
}