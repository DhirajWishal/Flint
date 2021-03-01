// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBuffer.h"
#include "Device.h"

namespace Flint
{
	namespace Backend
	{
		class RenderTarget;

		/**
		 * Command buffer manager object.
		 * This object is responsible for creating and maintaining command buffers.
		 */
		class CommandBufferManager : public BackendObject {
			friend CommandBuffer;

		public:
			CommandBufferManager() {}
			CommandBufferManager(Device* pDevice) : pDevice(pDevice) {}
			CommandBufferManager(Device* pDevice, std::vector<std::shared_ptr<CommandBuffer>>&& buffers) : pDevice(pDevice), mCommandBuffers(std::move(buffers)) {}

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

			/**
			 * Create a child command buffer manager object.
			 */
			virtual std::shared_ptr<CommandBufferManager> CreateChild(UI32 bufferCount, RenderTarget* pRenderTarget) = 0;

			virtual void UpdateChild(CommandBufferManager* pChildBufferManager, RenderTarget* pRenderTarget) = 0;

		public:
			/**
			 * Reset all the command buffers.
			 */
			virtual void Reset() = 0;

		public:
			std::shared_ptr<CommandBuffer> GetBuffer(UI64 index) { return mCommandBuffers[index]; }
			const std::shared_ptr<CommandBuffer> GetBuffer(UI64 index) const { return mCommandBuffers[index]; }
			std::vector<std::shared_ptr<CommandBuffer>>& GetBuffers() { return mCommandBuffers; }
			const std::vector<std::shared_ptr<CommandBuffer>> GetBuffers() const { return mCommandBuffers; }

		protected:
			std::vector<std::shared_ptr<CommandBuffer>> mCommandBuffers;

			Device* pDevice = nullptr;
		};
	}
}