// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBuffer.h"
#include "Device.h"

namespace Flint
{
	namespace Backend
	{
		class ScreenBoundRenderTarget;

		/**
		 * Command buffer manager object.
		 * This object is responsible for creating and maintaining command buffers.
		 */
		class CommandBufferManager : public BackendObject {
			friend CommandBuffer;

		public:
			CommandBufferManager() {}

			/**
			 * Create new command buffers.
			 *
			 * @param pDevice: The device to which the command buffers are bound to.
			 * @param count: The number of command buffers to create.
			 */
			virtual void CreateBuffers(Device* pDevice, UI32 count) = 0;

			/**
			 * Terminate the command buffer manager.
			 * This method terminates all the created command buffer objects.
			 */
			virtual void Terminate() = 0;

		public:
			/**
			 * Begin command buffer recording.
			 * Command buffers must begin recoding before performing tasks.
			 *
			 * @param index: The index of the buffer to be recorded.
			 */
			virtual void BeginBufferRecording(UI32 index) = 0;

			/**
			 * End command buffer recording.
			 * Command buffers must end recoding before submitting.
			 *
			 * @param index: The index of the buffer.
			 */
			virtual void EndBufferRecording(UI32 index) = 0;

			/**
			 * Begin command buffer execution.
			 *
			 * @param frameIndex: The frame index.
			 * @param pRenderTarget: The render target pointer.
			 * @return The image index.
			 */
			virtual I32 BeginCommandExecution(UI32 frameIndex, ScreenBoundRenderTarget* pRenderTarget) = 0;

			/**
			 * Submit commands to the GPU to be processed.
			 *
			 * @param index: The index of the buffer.
			 * @param pRenderTarget: The render target to be submitted to.
			 */
			virtual void SubmitCommand(UI32 index, ScreenBoundRenderTarget* pRenderTarget) = 0;

		public:
			/**
			 * Draw using index data.
			 * 
			 * @param index: The index of the command buffer.
			 * @param indexCount: The number of indexes to draw.
			 * @param vertexOffset: The vertex offset of the buffer.
			 * @param indexOffset: The offset to be added to the index buffer.
			 */
			virtual void DrawUsingIndexData(UI32 index, UI32 indexCount, UI32 vertexOffset, UI32 indexOffset) = 0;

		public:
			std::vector<CommandBuffer>& GetBuffers() { return mCommandBuffers; }
			const std::vector<CommandBuffer> GetBuffers() const { return mCommandBuffers; }

		protected:
			/**
			 * Resolve the command buffers.
			 *
			 * @param count: The number of command buffers to be created.
			 */
			void ResolveCommandBuffers(UI32 count);

		protected:
			std::vector<CommandBuffer> mCommandBuffers;

			Device* pDevice = nullptr;
		};
	}
}