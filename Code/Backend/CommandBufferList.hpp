// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"
#include "DynamicStateContainer.hpp"
#include "Buffer.hpp"

namespace Flint
{
	class ScreenBoundRenderTarget;
	class GraphicsPipeline;
	class ResourceMap;

	/**
	 * Flint command buffer list object.
	 * This object stored multiple command buffers which are used to submit commands to the GPU.
	 */
	class CommandBufferList : public DeviceBoundObject
	{
	public:
		/**
		 * Construct the command buffer list as a primary command buffer list.
		 *
		 * @param pDevice: The device to which the buffer list is bound to.
		 * @param bufferCount: The number of command buffers needed.
		 */
		CommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount);

		/**
		 * Construct the command buffer list as a secondary command buffer list.
		 * Secondary command buffer lists are used to multi-thread and to bind objects per frame.
		 *
		 * @param pDevice: The device object to which the buffer is bound to.
		 * @param bufferCount: The number of buffers needed.
		 * @param pParent: The parent command buffer list.
		 */
		CommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pParent) : DeviceBoundObject(pDevice), mBufferCount(bufferCount) {}

		/**
		 * Begin command buffer recording.
		 * Before binding objects to a command buffer, it must first start recording.
		 *
		 * @param index: The index of the command buffer.
		 */
		virtual void BeginBufferRecording(UI32 index) = 0;

		/**
		 * Begin secondary command buffer recording.
		 * Before binding objects to a command buffer, it must first start recording.
		 *
		 * @param index: The index of the command buffer.
		 * @param pParent: The parent command buffer pointer.
		 */
		virtual void BeginBufferRecording(UI32 index, const std::shared_ptr<CommandBufferList> pParent) = 0;

		/**
		 * Begin the next buffer recording.
		 */
		virtual void BeginNextBufferRecording() = 0;

		/**
		 * Begin the next buffer recording.
		 *
		 * @param pParent: The parent command buffer pointer.
		 */
		virtual void BeginNextBufferRecording(const std::shared_ptr<CommandBufferList> pParent) = 0;

		/**
		 * Bind a render target to the current command buffer.
		 *
		 * @param pRenderTarget: The render target pointer.
		 */
		virtual void BindRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) = 0;

		/**
		 * Bind a render target to the current command buffer as secondary.
		 *
		 * @param pRenderTarget: The render target pointer.
		 */
		virtual void BindRenderTargetSecondary(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) = 0;

		/**
		 * Bind a graphics pipeline to the current command buffer.
		 *
		 * @param pGraphicsPipeline: The graphics pipeline pointer.
		 */
		virtual void BindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline>& pGraphicsPipeline) = 0;

		/**
		 * Bind a vertex buffer to the current command buffer.
		 *
		 * @param pVertexBuffer: The vertex buffer to bind.
		 */
		virtual void BindVertexBuffer(const std::shared_ptr<Buffer>& pVertexBuffer) = 0;

		/**
		 * Bind an index buffer to the current command buffer.
		 *
		 * @param pIndexBuffer: The index buffer to bind.
		 * @param indexSize: The size of a single index.
		 */
		virtual void BindIndexBuffer(const std::shared_ptr<Buffer>& pIndexBuffer, UI64 indexSize) = 0;

		/**
		 * Bind draw resources to the current command buffer.
		 *
		 * @param pPipeline: The pipeline pointer.
		 * @param pResourceMap: The resource map to bind.
		 */
		virtual void BindDrawResources(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::shared_ptr<ResourceMap>& pResourceMap) = 0;

		/**
		 * Bind dynamic states to the current command buffer.
		 *
		 * @param pDynamicStates: The dynamic states to bind.
		 */
		virtual void BindDynamicStates(const std::shared_ptr<DynamicStateContainer>& pDynamicStates) = 0;

		/**
		 * Issue a draw call to the current command buffer.
		 *
		 * @param vertexOffset: The vertex offset of the vertex buffer to find the first vertex.
		 * @param vertexCount: The number of vertexes to draw.
		 * @param indexOffset: The index offset of the index buffer to find the first index.
		 * @param indexCount: The number of indexes to draw.
		 */
		virtual void IssueDrawCall(UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount) = 0;

		/**
		 * Execute all the secondary commands.
		 */
		virtual void ExecuteSecondaryCommands() = 0;

		/**
		 * Unbind the render target from the current buffer.
		 */
		virtual void UnbindRenderTarget() = 0;

		/**
		 * End the currently active command buffer recording.
		 */
		virtual void EndBufferRecording() = 0;

		/**
		 * Clear the command buffers.
		 */
		virtual void ClearBuffers() = 0;

	public:
		/**
		 * Get the command buffer list buffer count.
		 *
		 * @return The buffer count.
		 */
		const UI32 GetBufferCount() const { return mBufferCount; }

		/**
		 * Get the current buffer index.
		 *
		 * @return The index of the currently selected buffer.
		 */
		const UI32 GetCurrentBufferIndex() const { return mCurrentBufferIndex; }

		/**
		 * Check if the command buffer is recording.
		 * 
		 * @return The boolean value.
		 */
		const bool IsRecording() const { return bIsRecording; }

	protected:
		UI32 mBufferCount = 0;
		UI32 mCurrentBufferIndex = 0;

		bool bIsRecording = false;
	};
}