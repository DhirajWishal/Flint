// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/CommandBuffer.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBuffer final : public CommandBuffer, public std::enable_shared_from_this<VulkanCommandBuffer>
		{
		public:
			VulkanCommandBuffer(const std::shared_ptr<CommandBufferAllocator>& pAllocator, VkCommandBuffer vCommandBuffer);
			~VulkanCommandBuffer() { if (!bIsTerminated) Terminate(); }

			virtual void BeginBufferRecording() override final;
			virtual void BeginBufferRecording(const std::shared_ptr<ScreenBoundRenderTarget> pRenderTarget) override final;
			virtual void BeginBufferRecording(const std::shared_ptr<OffScreenRenderTarget> pRenderTarget) override final;

			virtual void BindRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) override final;
			virtual void BindRenderTargetSecondary(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) override final;
			virtual void BindRenderTarget(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget) override final;
			virtual void BindRenderTargetSecondary(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget) override final;
			virtual void UnbindRenderTarget() override final;
			virtual void BindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline>& pGraphicsPipeline) override final;
			virtual void BindComputePipeline(const std::shared_ptr<ComputePipeline>& pComputePipeline) override final;

			virtual void BindGeometryStore(const std::shared_ptr<GeometryStore>& pGeometryStore) override final;

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
			const VkSubmitInfo GetSubmitInfo() const { return vSubmitInfo; }
			void ResetFence();

			const VkSemaphore GetInFlightSemaphore() const { return vInFlight; }
			const VkSemaphore GetRenderFinishedSemaphore() const { return vRenderFinished; }
			const VkFence GetInFlightFence() const { return vInFlightFence; }

		private:
			VkSemaphoreWaitFlags vWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			std::vector<VkCommandBuffer> vSecondaryCommandBuffers = {};

			VkSemaphore vInFlight = VK_NULL_HANDLE;
			VkSemaphore vRenderFinished = VK_NULL_HANDLE;
			VkFence vInFlightFence = VK_NULL_HANDLE;

			VkCommandBuffer vCommandBuffer = VK_NULL_HANDLE;

			VkSubmitInfo vSubmitInfo = {};
		};
	}
}