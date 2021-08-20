// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBufferList.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferList final : public CommandBufferList, public std::enable_shared_from_this<VulkanCommandBufferList>
		{
		public:
			VulkanCommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount);
			VulkanCommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pParent);
			~VulkanCommandBufferList() { if (!bIsTerminated) Terminate(); }

			virtual std::shared_ptr<CommandBufferList> CreateChild() override final;

			virtual void BeginBufferRecording(UI32 index) override final;
			virtual void BeginBufferRecording(UI32 index, const std::shared_ptr<CommandBufferList> pParent) override final;
			virtual void BeginNextBufferRecording() override final;
			virtual void BeginNextBufferRecording(const std::shared_ptr<CommandBufferList> pParent) override final;

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
			virtual void ExecuteSecondaryCommands() override final;

			virtual void EndBufferRecording() override final;
			virtual void Terminate() override final;
			virtual void ClearBuffers() override final;

			VkCommandPool GetCommandPool() const { return vCommandPool; }
			VkCommandBuffer GetCurrentCommandBuffer() const { return vCommandBuffers[mCurrentBufferIndex]; }
			VkCommandBuffer GetCommandBuffer(UI32 index) const { return vCommandBuffers[index]; }
			std::vector<VkCommandBuffer> GetCommandBuffers() const { return vCommandBuffers; }

			VkCommandBufferInheritanceInfo GetInheritanceInfo() const;
			void AddSecondaryCommandBuffer(const VkCommandBuffer& vCommandBuffer) { vSecondaryCommandBuffers.push_back(vCommandBuffer); }
			void SetSecondaryCommandBuffers(std::vector<VkCommandBuffer>&& vSecondaryCommandBuffers);

		public:
			void VulkanBeginSecondaryCommandBuffer(UI32 bufferIndex, const VkCommandBufferInheritanceInfo* pInheritanceInfo);
			void VulkanBeginNextSecondaryCommandBuffer(const VkCommandBufferInheritanceInfo* pInheritanceInfo);
			void VulkanBindRenderTarget(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget, const VkFramebuffer vFrameBuffer);
			void VulkanBindRenderTargetSecondary(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget, const VkFramebuffer vFrameBuffer);

			void SubmitComputeCommands();

		private:
			VkCommandPool vCommandPool = VK_NULL_HANDLE;
			std::vector<VkCommandBuffer> vCommandBuffers = {};
			std::vector<VkCommandBuffer> vSecondaryCommandBuffers = {};
		};
	}
}