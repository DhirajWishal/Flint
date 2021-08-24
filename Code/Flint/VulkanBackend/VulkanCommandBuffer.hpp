// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBuffer.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBuffer final : public CommandBuffer
		{
		public:
			VulkanCommandBuffer(const std::shared_ptr<CommandBufferAllocator>& pAllocator, VkCommandBuffer vCommandBuffer);
			~VulkanCommandBuffer() { if (!bIsTerminated) Terminate(); }

			virtual void BeginBufferRecording() override final;
			virtual void BeginBufferRecording(const std::shared_ptr<CommandBufferList> pParent) override final;

			virtual void BindRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) override final;
			virtual void BindRenderTargetSecondary(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) override final;
			virtual void BindRenderTarget(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget) override final;
			virtual void BindRenderTargetSecondary(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget) override final;
			virtual void UnbindRenderTarget() override final;
			virtual void BindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline>& pGraphicsPipeline) override final;
			virtual void BindComputePipeline(const std::shared_ptr<ComputePipeline>& pComputePipeline) override final;

			virtual void BindVertexBuffer(const std::shared_ptr<Buffer>& pVertexBuffer) override final;
			virtual void BindIndexBuffer(const std::shared_ptr<Buffer>& pIndexBuffer, UI64 indexSize) override final;

			virtual void BindResourceMap(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::shared_ptr<ResourceMap>& pResourceMap) override final;
			virtual void BindResourceMap(const std::shared_ptr<ComputePipeline>& pPipeline, const std::shared_ptr<ResourceMap>& pResourceMap) override final;
			virtual void BindDynamicStates(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::shared_ptr<DynamicStateContainer>& pDynamicStates) override final;
			virtual void BindDynamicStates(const std::shared_ptr<ComputePipeline>& pPipeline, const std::shared_ptr<DynamicStateContainer>& pDynamicStates) override final;

			virtual void IssueDrawCall(UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount) override final;
			virtual void IssueComputeCall(const FBox3D& groups) override final;
			virtual void SubmitSecondaryCommandBuffer(const std::shared_ptr<CommandBuffer>& pCommandBuffer) override final;
			virtual void ExecuteSecondaryCommands() override final;

			virtual void EndBufferRecording() override final;

			virtual void Terminate() override final;

		public:
			const VkCommandBuffer GetVulkanCommandBuffer() const { return vCommandBuffer; }

			const VkSemaphore GetInFlightSemaphore() const { return vInFlight; }
			const VkSemaphore GetRenderFinishedSemaphore() const { return vRenderFinished; }
			const VkFence GetInFlightFence() const { return vInFlightFence; }

		private:
			std::vector<VkCommandBuffer> vSecondaryCommandBuffers = {};

			VkSemaphore vInFlight = VK_NULL_HANDLE;
			VkSemaphore vRenderFinished = VK_NULL_HANDLE;
			VkFence vInFlightFence = VK_NULL_HANDLE;

			VkCommandBuffer vCommandBuffer = VK_NULL_HANDLE;
		};
	}
}