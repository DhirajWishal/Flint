// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/CommandBuffer.hpp"
#include "VulkanDevice.hpp"

#include "VulkanBackend/VulkanCommandBufferAllocator.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanComputePipeline.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanImage.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"
#include "VulkanBackend/VulkanResourcePackage.hpp"
#include "VulkanBackend/VulkanQuery.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBuffer final : public CommandBuffer<VulkanCommandBuffer, VulkanCommandBufferAllocator, VulkanScreenBoundRenderTarget, VulkanOffScreenRenderTarget, VulkanGraphicsPipeline, VulkanComputePipeline, VulkanBuffer, VulkanResourcePackage, VulkanImage, VulkanSwapChain, VulkanQuery>
		{
		public:
			VulkanCommandBuffer(VulkanCommandBufferAllocator* pAllocator, VkCommandBuffer vCommandBuffer);
			~VulkanCommandBuffer() { if (!bIsTerminated) Terminate(); }

			virtual void BeginBufferRecording() override;
			virtual void BeginBufferRecording(const VulkanScreenBoundRenderTarget* pRenderTarget) override;
			virtual void BeginBufferRecording(const VulkanOffScreenRenderTarget* pRenderTarget) override;

			virtual void BindRenderTarget(const VulkanScreenBoundRenderTarget* pRenderTarget) override;
			virtual void BindRenderTargetSecondary(const VulkanScreenBoundRenderTarget* pRenderTarget) override;
			virtual void BindRenderTarget(const VulkanOffScreenRenderTarget* pRenderTarget) override;
			virtual void BindRenderTargetSecondary(const VulkanOffScreenRenderTarget* pRenderTarget) override;
			virtual void ClearRenderTarget(const VulkanScreenBoundRenderTarget* pRenderTarget) override;
			virtual void ClearRenderTarget(const VulkanOffScreenRenderTarget* pRenderTarget) override;
			virtual void UnbindRenderTarget() override;
			virtual void BindGraphicsPipeline(const VulkanGraphicsPipeline* pGraphicsPipeline) override;
			virtual void BindComputePipeline(const VulkanComputePipeline* pComputePipeline) override;

			virtual void BindVertexBuffer(const VulkanBuffer* pBuffer, const uint64_t firstBinding = 0, const uint64_t offset = 0) override;
			virtual void BindIndexBuffer(const VulkanBuffer* pBuffer, const uint64_t indexSize, const uint64_t offset = 0) override;
			virtual void BindGeometryStore(const GeometryStore* pGeometryStore) override;

			virtual void BindResourcePackage(const VulkanGraphicsPipeline* pPipeline, VulkanResourcePackage* pResourcePackage) override;
			virtual void BindResourcePackages(const VulkanGraphicsPipeline* pPipeline, const std::vector<VulkanResourcePackage*>& pResourcePackages) override;
			virtual void BindResourcePackage(const VulkanComputePipeline* pPipeline, VulkanResourcePackage* pResourcePackage) override;
			virtual void BindResourcePackages(const VulkanComputePipeline* pPipeline, const std::vector<VulkanResourcePackage*>& pResourcePackages) override;
			virtual void BindDynamicStates(const VulkanGraphicsPipeline* pPipeline, const DynamicStateContainer* pDynamicStates) override;
			virtual void BindViewPort(const VulkanGraphicsPipeline* pPipeline, const ViewPort* pViewPort) override;
			virtual void BindScissor(const VulkanGraphicsPipeline* pPipeline, const Scissor* pScissor) override;
			virtual void BindLineWidth(const VulkanGraphicsPipeline* pPipeline, const LineWidth* pLineWidth) override;
			virtual void BindDepthBias(const VulkanGraphicsPipeline* pPipeline, const DepthBias* pDepthBias) override;
			virtual void BindBlendConstants(const VulkanGraphicsPipeline* pPipeline, const BlendConstants* pBlendConstants) override;
			virtual void BindDepthBounds(const VulkanGraphicsPipeline* pPipeline, const DepthBounds* pDepthBounds) override;
			virtual void BindConstantData(const VulkanGraphicsPipeline* pPipeline, const ConstantData* pConstantData, const ShaderType type) override;
			virtual void BindDynamicStates(const VulkanComputePipeline* pPipeline, const DynamicStateContainer* pDynamicStates) override;
			virtual void BindConstantData(const VulkanComputePipeline* pPipeline, const ConstantData* pConstantData) override;

			virtual void IssueDrawCall(WireFrame& wireFrame, const uint64_t firstInstance = 0, const uint64_t instanceCount = 1, const DrawCallMode mode = DrawCallMode::Indexed) override;
			virtual void IssueDrawCall(WireFrame&& wireFrame, const uint64_t firstInstance = 0, const uint64_t instanceCount = 1, const DrawCallMode mode = DrawCallMode::Indexed) override;
			virtual void IssueComputeCall(const FBox3D& groups) override;

			virtual void CopyImage(const VulkanImage* pSourceImage, const FBox3D sourceOffset, VulkanImage* pDestinationImage, const FBox3D destinationOffset) override;
			virtual void CopyToSwapChainImage(const VulkanImage* pSourceImage, const FBox3D sourceOffset, VulkanSwapChain* pSwapChain, const uint32_t imageIndex, const FBox3D destinationOffset) override;

			virtual void SubmitSecondaryCommandBuffer(const std::shared_ptr<VulkanCommandBuffer>& pCommandBuffer) override;
			virtual void ExecuteSecondaryCommands() override;

			virtual void EndBufferRecording() override;

			virtual void IncludeSwapChain(VulkanSwapChain* pSwapChain) override;

			virtual void BeginQuery(const VulkanQuery* pQuery, const uint32_t index, const bool requirePrecision = false) override;
			virtual void EndQuery(const VulkanQuery* pQuery, const uint32_t index) override;
			virtual void ResetQuery(const VulkanQuery* pQuery, const uint32_t beginIndex, const uint32_t count) override;

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