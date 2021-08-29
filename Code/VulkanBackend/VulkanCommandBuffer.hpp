// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/CommandBuffer.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBuffer final : public CommandBuffer, public boost::enable_shared_from_this<VulkanCommandBuffer>
		{
		public:
			VulkanCommandBuffer(const boost::shared_ptr<CommandBufferAllocator>& pAllocator, VkCommandBuffer vCommandBuffer);
			~VulkanCommandBuffer() { if (!bIsTerminated) Terminate(); }

			virtual void BeginBufferRecording() override final;
			virtual void BeginBufferRecording(const boost::shared_ptr<ScreenBoundRenderTarget> pRenderTarget) override final;
			virtual void BeginBufferRecording(const boost::shared_ptr<OffScreenRenderTarget> pRenderTarget) override final;

			virtual void BindRenderTarget(const boost::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) override final;
			virtual void BindRenderTargetSecondary(const boost::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) override final;
			virtual void BindRenderTarget(const boost::shared_ptr<OffScreenRenderTarget>& pRenderTarget) override final;
			virtual void BindRenderTargetSecondary(const boost::shared_ptr<OffScreenRenderTarget>& pRenderTarget) override final;
			virtual void UnbindRenderTarget() override final;
			virtual void BindGraphicsPipeline(const boost::shared_ptr<GraphicsPipeline>& pGraphicsPipeline) override final;
			virtual void BindComputePipeline(const boost::shared_ptr<ComputePipeline>& pComputePipeline) override final;

			virtual void BindGeometryStore(const boost::shared_ptr<GeometryStore>& pGeometryStore) override final;

			virtual void BindResourceMap(const boost::shared_ptr<GraphicsPipeline>& pPipeline, const boost::shared_ptr<ResourceMap>& pResourceMap) override final;
			virtual void BindResourceMap(const boost::shared_ptr<ComputePipeline>& pPipeline, const boost::shared_ptr<ResourceMap>& pResourceMap) override final;
			virtual void BindDynamicStates(const boost::shared_ptr<GraphicsPipeline>& pPipeline, const boost::shared_ptr<DynamicStateContainer>& pDynamicStates) override final;
			virtual void BindDynamicStates(const boost::shared_ptr<ComputePipeline>& pPipeline, const boost::shared_ptr<DynamicStateContainer>& pDynamicStates) override final;

			virtual void IssueDrawCall(UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount) override final;
			virtual void IssueComputeCall(const FBox3D& groups) override final;
			virtual void SubmitSecondaryCommandBuffer(const boost::shared_ptr<CommandBuffer>& pCommandBuffer) override final;
			virtual void ExecuteSecondaryCommands() override final;

			virtual void EndBufferRecording() override final;

			virtual void Terminate() override final;

		public:
			const VkCommandBuffer GetVulkanCommandBuffer() const { return vCommandBuffer; }
			const VkSubmitInfo GetSubmitInfo();
			void ResetFence();

			const VkFence GetInFlightFence() const { return vInFlightFence; }

		private:
			VkSemaphoreWaitFlags vWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			boost::container::vector<VkCommandBuffer> vSecondaryCommandBuffers = {};

			boost::container::vector<VkSemaphore> vInFlightSemaphores = {};
			boost::container::vector<VkSemaphore> vRenderFinishedSemaphores = {};

			VkFence vInFlightFence = VK_NULL_HANDLE;

			VkCommandBuffer vCommandBuffer = VK_NULL_HANDLE;

			VkSubmitInfo vSubmitInfo = {};
		};
	}
}