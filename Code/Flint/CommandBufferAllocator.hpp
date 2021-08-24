// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	class CommandBuffer;

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
		 * @param pDevice: The device pointer.
		 * @param bufferCount: The number of buffers to allocate.
		 */
		CommandBufferAllocator(const std::shared_ptr<Device>& pDevice, UI32 bufferCount);

		/**
		 * Construct a secondary command buffer allocator.
		 *
		 * @param pDevice: The device pointer.
		 * @param pParent: The parent command buffer allocator.
		 * @param bufferCount: The number of buffers to allocate.
		 */
		CommandBufferAllocator(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<CommandBufferAllocator>& pParent, UI32 bufferCount);

	public:
		/**
		 * Get the allocated buffer count.
		 * 
		 * @return The buffer count.
		 */
		const UI32 GetBufferCount() const { return mBufferCount; }

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

	protected:
		/**
		 * Flag the command buffer as terminated.
		 * 
		 * @param index: The command buffer index.
		 */
		void FlagCommandBufferAsTerminated(UI32 index) { pCommandBuffers[index]->FlagAsTerminated(); }

	protected:
		std::vector<std::shared_ptr<CommandBuffer>> pCommandBuffers = {};
		std::shared_ptr<CommandBufferAllocator> pParentAllocator = nullptr;

		UI32 mBufferCount = 0;
	};
}