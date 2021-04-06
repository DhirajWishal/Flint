// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Buffer.h"
#include "Pipeline.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint command buffer list.
		 * Commands are submitted to the device using command buffer lists. Each list is made of command buffers which are handled internally.
		 *
		 * @tparam TDevice: The device type.
		 * @tparam TBuffer: The buffer type.
		 * @tparam TScreenBoundRenderTarget: The screen bound render target type.
		 * @tparam TGraphicsPipeline: The graphics pipeline type.
		 * @tparam TPipelineResource: The render resource type.
		 */
		template<class TDevice, class TBuffer, class TScreenBoundRenderTarget, class TGraphicsPipeline, class TPipelineResource>
		class CommandBufferList : public BackendObject {
		public:
			using DeviceType = TDevice;
			using BufferType = TBuffer;
			using ScreenBoundRenderTargetType = TScreenBoundRenderTarget;
			using GraphicsPipelineType = TGraphicsPipeline;
			using PipelineResourceType = TPipelineResource;

		public:
			CommandBufferList() {}
			virtual ~CommandBufferList() {}

			/**
			 * Initialize the list.
			 *
			 * @param pDevice: The device pointer.
			 * @param bufferCount: The buffer count.
			 */
			virtual void Initialize(DeviceType* pDevice, UI64 bufferCount) = 0;

			/**
			 * Terminate the buffer list.
			 */
			virtual void Terminate() = 0;

			/**
			 * Clear command buffer recordings.
			 * This resets them back to a unrecorded state.
			 */
			virtual void ClearBuffers() = 0;

			/**
			 * Recreate command buffers.
			 */
			virtual void ReceateBuffers() = 0;

			/**
			 * Begin command buffer recording.
			 * This must be done prior to binding anything to a buffer.
			 *
			 * @param index: The index of the buffer.
			 */
			virtual void BeginBuffer(UI64 index) = 0;

			/**
			 * Begin the next command buffer.
			 */
			virtual void BeginNextBuffer() { BeginBuffer(IncrementIndex()); }

			/**
			 * End the current command buffer recording.
			 */
			virtual void EndBuffer() = 0;

			/**
			 * Bind a vertex buffer to the command buffer.
			 *
			 * @param buffer: The vertex buffer.
			 */
			virtual void BindVertexBuffer(const BufferType& buffer) = 0;

			/**
			 * Bind a index buffer to the command buffer.
			 *
			 * @param buffer: The index buffer.
			 */
			virtual void BindIndexBuffer(const BufferType& buffer) = 0;

			/**
			 * Bind a screen bound render target to the command buffer.
			 *
			 * @param renderTarget: The render target to be bound.
			 */
			virtual void BindRenderTarget(const ScreenBoundRenderTargetType& renderTarget) = 0;

			/**
			 * End the render target.
			 */
			virtual void EndRenderTarget() = 0;

			/**
			 * Bind a graphics pipeline to the command buffer.
			 *
			 * @param graphicsPipeline: The graphics pipeline.
			 */
			virtual void BindPipeline(const GraphicsPipelineType& graphicsPipeline) = 0;

			/**
			 * Set dynamic states of the pipeline.
			 * 
			 * @param container: The dynamic state container.
			 */
			virtual void SetDynamicStates(const DynamicStateContainer& container) = 0;

			/**
			 * Bind render resources to the pipeline.
			 * 
			 * @param resource: The render resource.
			 */
			virtual void BindRenderResource(const PipelineResourceType& resource) = 0;

			/**
			 * Draw using the index buffer.
			 * 
			 * @param indexCount: The number of indexes to draw.
			 * @param indexOffset: The offset of the index buffer.
			 * @param vertexOffset: The vertex buffer offset.
			 */
			virtual void DrawIndexed(UI64 indexCount, UI64 indexOffset, UI64 vertexOffset) = 0;

		public:
			/**
			 * Get the buffer count of the list.
			 * 
			 * @return The buffer count.
			 */
			UI64 GetBufferCount() const { return mBufferCount; }

			/**
			 * Get the current buffer index.
			 * 
			 * @return The index of the buffer which is active.
			 */
			UI64 GetIndex() const { return mBufferIndex; }

			/**
			 * Set a new buffer index.
			 * 
			 * @param index: The index to be set.
			 */
			void SetIndex(UI64 index) { mBufferIndex = index; }

			/**
			 * Increment the index by one.
			 * If the index is greater than or equal to the buffer count, it will be set back to 0.
			 * 
			 * @return The incremented buffer index.
			 */
			UI64 IncrementIndex()
			{
				mBufferIndex++;
				if (mBufferIndex >= mBufferCount) mBufferIndex = 0;
				return mBufferIndex;
			}

		protected:
			DeviceType* pDevice = nullptr;
			UI64 mBufferCount = 0;
			UI64 mBufferIndex = 0;
		};
	}
}