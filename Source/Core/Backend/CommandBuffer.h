// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendObject.h"

#include <memory>

namespace Flint
{
	namespace Backend
	{
		class CommandBufferManager;
		class ScreenBoundRenderTarget;
		struct DynamicStateContainer;

		/**
		 * Command buffer object.
		 * This object is used to submit draw and other utility commands to the GPU device.
		 */
		class CommandBuffer : public BackendObject {
			friend CommandBufferManager;

		public:
			CommandBuffer() {}

			/**
			 * Begin command buffer recording.
			 * Command buffers must begin recoding before performing tasks.
			 */
			virtual void BeginBufferRecording() = 0;

			/**
			 * End command buffer recording.
			 * Command buffers must end recoding before submitting.
			 *
			 * @param index: The index of the buffer.
			 */
			virtual void EndBufferRecording() = 0;

		public:
			/**
			 * Submit dynamic states to the command buffer.
			 *
			 * @param index: The index of the command buffer.
			 * @param container: The dynamic state container.
			 */
			virtual void SubmitDynamicStates(const DynamicStateContainer& container) = 0;

			/**
			 * Draw using index data.
			 *
			 * @param index: The index of the command buffer.
			 * @param indexCount: The number of indexes to draw.
			 * @param vertexOffset: The vertex offset of the buffer.
			 * @param indexOffset: The offset to be added to the index buffer.
			 */
			virtual void DrawUsingIndexData(UI32 indexCount, UI32 vertexOffset, UI32 indexOffset) = 0;

			/**
			 * Execute child commands using the command buffer.
			 * 
			 * @param pCommandBuffers: The command buffers to be executed.
			 */
			virtual void ExecuteChildCommands(const std::vector<std::shared_ptr<CommandBuffer>>& pCommandBuffers) = 0;

		protected:
			CommandBufferManager* pParent = nullptr;
		};
	}
}