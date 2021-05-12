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
		class VulkanScreenBoundRenderTarget;
		class VulkanGraphicsPipeline;

		class VulkanCommandBufferList final : public Backend::CommandBufferList
		{
		public:
			VulkanCommandBufferList(Backend::Device* pDevice, UI64 bufferCount);

			virtual void Terminate() override final;
			virtual void ClearBuffers() override final;
			virtual void ReceateBuffers() override final;

			virtual void BeginBuffer(UI64 index) override final;
			virtual void EndBuffer() override final;

			virtual void BindVertexBuffer(const Backend::Buffer* pBuffer) override final;
			virtual void BindIndexBuffer(const Backend::Buffer* pBuffer) override final;
			virtual void BindRenderTarget(const Backend::ScreenBoundRenderTarget* pRenderTarget) override final;
			virtual void EndRenderTarget() override final;
			virtual void BindPipeline(const Backend::GraphicsPipeline* pGraphicsPipeline) override final;
			virtual void SetDynamicStates(const Backend::DynamicStateContainer& container) override final;
			virtual void BindRenderResource(const Backend::PipelineResource* pResource) override final;
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