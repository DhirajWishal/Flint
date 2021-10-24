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
			virtual void BeginBufferRecording(const ScreenBoundRenderTarget* pRenderTarget) override;
			virtual void BeginBufferRecording(const OffScreenRenderTarget* pRenderTarget) override;

			virtual void BindRenderTarget(const ScreenBoundRenderTarget* pRenderTarget) override;
			virtual void BindRenderTargetSecondary(const ScreenBoundRenderTarget* pRenderTarget) override;
			virtual void BindRenderTarget(const OffScreenRenderTarget* pRenderTarget) override;
			virtual void BindRenderTargetSecondary(const OffScreenRenderTarget* pRenderTarget) override;
			virtual void ClearRenderTarget(const ScreenBoundRenderTarget* pRenderTarget) override;
			virtual void ClearRenderTarget(const OffScreenRenderTarget* pRenderTarget) override;
			virtual void UnbindRenderTarget() override;
			virtual void BindGraphicsPipeline(const GraphicsPipeline* pGraphicsPipeline) override;
			virtual void BindComputePipeline(const ComputePipeline* pComputePipeline) override;

			virtual void BindVertexBuffer(const Buffer* pBuffer, const UI64 firstBinding = 0, const UI64 offset = 0) override;
			virtual void BindIndexBuffer(const Buffer* pBuffer, const UI64 indexSize, const UI64 offset = 0) override;
			virtual void BindGeometryStore(const GeometryStore* pGeometryStore) override;

			virtual void BindResourcePackage(const GraphicsPipeline* pPipeline, ResourcePackage* pResourcePackage) override;
			virtual void BindResourcePackages(const GraphicsPipeline* pPipeline, const std::vector<ResourcePackage*>& pResourcePackages) override;
			virtual void BindResourcePackage(const ComputePipeline* pPipeline, ResourcePackage* pResourcePackage) override;
			virtual void BindResourcePackages(const ComputePipeline* pPipeline, const std::vector<ResourcePackage*>& pResourcePackages) override;
			virtual void BindDynamicStates(const GraphicsPipeline* pPipeline, const DynamicStateContainer* pDynamicStates) override;
			virtual void BindViewPort(const GraphicsPipeline* pPipeline, const ViewPort* pViewPort) override;
			virtual void BindScissor(const GraphicsPipeline* pPipeline, const Scissor* pScissor) override;
			virtual void BindLineWidth(const GraphicsPipeline* pPipeline, const LineWidth* pLineWidth) override;
			virtual void BindDepthBias(const GraphicsPipeline* pPipeline, const DepthBias* pDepthBias) override;
			virtual void BindBlendConstants(const GraphicsPipeline* pPipeline, const BlendConstants* pBlendConstants) override;
			virtual void BindDepthBounds(const GraphicsPipeline* pPipeline, const DepthBounds* pDepthBounds) override;
			virtual void BindConstantData(const GraphicsPipeline* pPipeline, const ConstantData* pConstantData, const ShaderType type) override;
			virtual void BindDynamicStates(const ComputePipeline* pPipeline, const DynamicStateContainer* pDynamicStates) override;
			virtual void BindConstantData(const ComputePipeline* pPipeline, const ConstantData* pConstantData) override;

			virtual void IssueDrawCall(WireFrame& wireFrame, const UI64 firstInstance = 0, const UI64 instanceCount = 1, const DrawCallMode mode = DrawCallMode::Indexed) override;
			virtual void IssueDrawCall(WireFrame&& wireFrame, const UI64 firstInstance = 0, const UI64 instanceCount = 1, const DrawCallMode mode = DrawCallMode::Indexed) override;
			virtual void IssueComputeCall(const FBox3D& groups) override;

			virtual void CopyImage(const Image* pSourceImage, const FBox3D sourceOffset, Image* pDestinationImage, const FBox3D destinationOffset) override;
			virtual void CopyToSwapChainImage(const Image* pSourceImage, const FBox3D sourceOffset, SwapChain* pSwapChain, const UI32 imageIndex, const FBox3D destinationOffset) override;

			virtual void SubmitSecondaryCommandBuffer(const std::shared_ptr<CommandBuffer>& pCommandBuffer) override;
			virtual void ExecuteSecondaryCommands() override;

			virtual void EndBufferRecording() override;

			virtual void IncludeSwapChain(SwapChain* pSwapChain) override;

			virtual void BeginQuery(const Query* pQuery, const UI32 index, const bool requirePrecision = false) override;
			virtual void EndQuery(const Query* pQuery, const UI32 index) override;
			virtual void ResetQuery(const Query* pQuery, const UI32 beginIndex, const UI32 count) override;

			virtual void Synchronize() override;

			virtual void Terminate() override;

		public:
			const VkCommandBuffer GetVulkanCommandBuffer() const { return vCommandBuffer; }
			const VkSubmitInfo GetSubmitInfo() const;
			const VkSubmitInfo* GetSubmitInfoAddress() const;

		private:
			std::vector<VkCommandBuffer> vSecondaryCommandBuffers = {};

			std::vector<VkSemaphore> vInFlightSemaphores = {};
			std::vector<VkSemaphore> vRenderFinishedSemaphores = {};

			mutable VkSubmitInfo vSubmitInfo = {};

			VkCommandBuffer vCommandBuffer = VK_NULL_HANDLE;

			VkSemaphoreWaitFlags vWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		};
	}
}