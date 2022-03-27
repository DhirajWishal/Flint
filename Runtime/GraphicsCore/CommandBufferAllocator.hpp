// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBuffer.hpp"

namespace Flint
{
	/**
	 * Flint command buffer allocator.
	 * These allocators are used to allocate command buffers and are responsible for the command buffers' lifetime.
	 * 
	 * There are two main types of command buffer allocators. 
	 * 1. Primary.
	 * 2. Secondary.
	 */
	class CommandBufferAllocator : public DeviceBoundObject
	{
	public:
		/**
		 * Construct a primary command buffer allocator.
		 * 
		 * @param pDevice The device pointer.
		 * @param bufferCount The number of buffers to allocate.
		 */
		CommandBufferAllocator(Device* pDevice, const uint32_t bufferCount);

		/**
		 * Construct a secondary command buffer allocator.
		 *
		 * @param pDevice The device pointer.
		 * @param pParent The parent command buffer allocator.
		 * @param bufferCount The number of buffers to allocate.
		 */
		CommandBufferAllocator(Device* pDevice, const std::shared_ptr<CommandBufferAllocator>& pParent, const uint32_t bufferCount);

		/**
		 * Create the command buffers.
		 * 
		 * @return The created command buffers.
		 */
		virtual const std::vector<std::shared_ptr<CommandBuffer>> CreateCommandBuffers() = 0;

		/**
		 * Create a child allocator object.
		 * 
		 * @return The created secondary command buffer allocator.
		 */
		virtual std::shared_ptr<CommandBufferAllocator> CreateChildAllocator() = 0;

	public:
		/**
		 * Get the allocated buffer count.
		 * 
		 * @return The buffer count.
		 */
		const uint32_t GetBufferCount() const { return mBufferCount; }

		/**
		 * Get the parent command buffer pointer.
		 * 
		 * @return The parent pointer. Returns nullptr if this is a primary command buffer allocator.
		 */
		const std::shared_ptr<CommandBufferAllocator> GetParentAllocator() const { return pParentAllocator; }

		/**
		 * Get the allocated command buffers.
		 * 
		 * @return The command buffer pointers.
		 */
		const std::vector<std::shared_ptr<CommandBuffer>> GetCommandBuffers() const { return pCommandBuffers; }

		/**
		 * Get a command buffer from the allocator.
		 * 
		 * @param index The index of the command buffer.
		 * @return The command buffer pointer.
		 */
		const std::shared_ptr<CommandBuffer> GetCommandBuffer(const uint64_t index) const { return pCommandBuffers[index]; }

	protected:
		/**
		 * Flag the command buffer as terminated.
		 * 
		 * @param index The command buffer index.
		 */
		void FlagCommandBufferAsTerminated(uint32_t index) { pCommandBuffers[index]->Terminate(); }

	protected:
		std::vector<std::shared_ptr<CommandBuffer>> pCommandBuffers = {};
		std::shared_ptr<CommandBufferAllocator> pParentAllocator = nullptr;

		uint32_t mBufferCount = 0;
	};
}