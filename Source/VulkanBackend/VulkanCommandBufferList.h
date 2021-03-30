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
		template<class TRenderTarget>
		class VulkanGraphicsPipeline;

		class VulkanCommandBufferList final : public Backend::CommandBufferList<VulkanDevice, VulkanBuffer, VulkanScreenBoundRenderTargetS, VulkanGraphicsPipeline<VulkanScreenBoundRenderTargetS>, VulkanPipelineResource<VulkanGraphicsPipeline<VulkanScreenBoundRenderTargetS>>> {
		public:
			using DeviceType = VulkanDevice;
			using BufferType = VulkanBuffer;
			using ScreenBoundRenderTargetType = VulkanScreenBoundRenderTargetS;
			using ScreenBoundGraphicsPipelineType = VulkanGraphicsPipeline<VulkanScreenBoundRenderTargetS>;
			using ScreenBoundGraphicsPipelineResourceType = VulkanPipelineResource<VulkanGraphicsPipeline<VulkanScreenBoundRenderTargetS>>;
		
		public:
			VulkanCommandBufferList() {}
			~VulkanCommandBufferList() {}

			virtual void Initialize(DeviceType* pDevice, UI64 bufferCount) override final;
			virtual void Terminate() override final;

			virtual void ClearBuffers() override final;
			virtual void ReCreateBuffers() override final;

			virtual void BeginBuffer(UI64 index) override final;
			virtual void EndBuffer() override final;

			virtual void BindVertexBuffer(const BufferType& buffer, UI64 firstBinding, UI64 bindingCount) override final;
			virtual void BindIndexBuffer(const BufferType& buffer) override final;
			virtual void BindRenderTarget(const ScreenBoundRenderTargetType& renderTarget) override final;
			virtual void EndRenderTarget() override final;
			virtual void BindPipeline(const ScreenBoundGraphicsPipelineType& graphicsPipeline) override final;
			virtual void SetDynamicStates(const Backend::DynamicStateContainer& container) override final;
			virtual void BindRenderResource(const ScreenBoundGraphicsPipelineResourceType& resource) override final;
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