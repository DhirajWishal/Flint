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

			virtual void BeginBufferRecording() override;
			virtual void BeginBufferRecording(const std::shared_ptr<ScreenBoundRenderTarget> pRenderTarget) override;
			virtual void BeginBufferRecording(const std::shared_ptr<OffScreenRenderTarget> pRenderTarget) override;

			virtual void BindRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) override;
			virtual void BindRenderTargetSecondary(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) override;
			virtual void BindRenderTarget(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget) override;
			virtual void BindRenderTargetSecondary(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget) override;
			virtual void UnbindRenderTarget() override;
			virtual void BindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline>& pGraphicsPipeline) override;
			virtual void BindComputePipeline(const std::shared_ptr<ComputePipeline>& pComputePipeline) override;

			virtual void BindGeometryStore(const std::shared_ptr<GeometryStore>& pGeometryStore) override;

			virtual void BindResourcePackages(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::vector<std::shared_ptr<ResourcePackage>>& pResourcePackages) override;
			virtual void BindResourcePackages(const std::shared_ptr<ComputePipeline>& pPipeline, const std::vector<std::shared_ptr<ResourcePackage>>& pResourcePackages) override;
			virtual void BindDynamicStates(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::shared_ptr<DynamicStateContainer>& pDynamicStates) override;
			virtual void BindDynamicStates(const std::shared_ptr<ComputePipeline>& pPipeline, const std::shared_ptr<DynamicStateContainer>& pDynamicStates) override;

			virtual void IssueDrawCall(const WireFrame& wireFrame, const DrawCallMode mode = DrawCallMode::Indexed) override;
			virtual void IssueComputeCall(const FBox3D& groups) override;
			virtual void SubmitSecondaryCommandBuffer(const std::shared_ptr<CommandBuffer>& pCommandBuffer) override;
			virtual void ExecuteSecondaryCommands() override;

			virtual void EndBufferRecording() override;

			virtual void Terminate() override;

		public:
			const VkCommandBuffer GetVulkanCommandBuffer() const { return vCommandBuffer; }
			const VkSubmitInfo GetSubmitInfo();
			void ResetFence();

			const VkFence GetInFlightFence() const { return vInFlightFence; }

		private:
			std::vector<VkCommandBuffer> vSecondaryCommandBuffers = {};

			std::vector<VkSemaphore> vInFlightSemaphores = {};
			std::vector<VkSemaphore> vRenderFinishedSemaphores = {};

			VkSubmitInfo vSubmitInfo = {};

			VkFence vInFlightFence = VK_NULL_HANDLE;
			VkCommandBuffer vCommandBuffer = VK_NULL_HANDLE;

			VkSemaphoreWaitFlags vWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		};
	}
}