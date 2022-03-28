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
	template<class DerivedT, class DeviceT, class CommandBufferT>
	class CommandBufferAllocator : public DeviceBoundObject<DeviceT>
	{
	public:
		/**
		 * Construct a primary command buffer allocator.
		 * 
		 * @param pDevice The device pointer.
		 * @param bufferCount The number of buffers to allocate.
		 */
		CommandBufferAllocator(DeviceT* pDevice, const uint32_t bufferCount) : DeviceBoundObject(pDevice), m_BufferCount(bufferCount)
		{
			if (bufferCount == 0)
				throw std::invalid_argument("Command buffer count should not be 0!");
		}

		/**
		 * Construct a secondary command buffer allocator.
		 *
		 * @param pDevice The device pointer.
		 * @param pParent The parent command buffer allocator.
		 * @param bufferCount The number of buffers to allocate.
		 */
		CommandBufferAllocator(DeviceT* pDevice, DerivedT* pParent, const uint32_t bufferCount) : DeviceBoundObject(pDevice), pParentAllocator(pParent), m_BufferCount(bufferCount)
		{
			if (!pParentAllocator)
				throw std::invalid_argument("The parent command buffer allocator pointer should not be null!");

			if (bufferCount == 0)
				throw std::invalid_argument("Command buffer count should not be 0!");
		}

		/**
		 * Create the command buffers.
		 * 
		 * @return The created command buffers.
		 */
		virtual const std::vector<std::shared_ptr<CommandBufferT>> CreateCommandBuffers() = 0;

		/**
		 * Create a child allocator object.
		 * 
		 * @return The created secondary command buffer allocator.
		 */
		virtual std::unique_ptr<DerivedT> CreateChildAllocator() = 0;

	public:
		/**
		 * Get the allocated buffer count.
		 * 
		 * @return The buffer count.
		 */
		const uint32_t GetBufferCount() const { return m_BufferCount; }

		/**
		 * Get the parent command buffer pointer.
		 * 
		 * @return The parent pointer. Returns nullptr if this is a primary command buffer allocator.
		 */
		const DerivedT* GetParentAllocator() const { return pParentAllocator; }

		/**
		 * Get the allocated command buffers.
		 * 
		 * @return The command buffer pointers.
		 */
		const std::vector<std::shared_ptr<CommandBufferT>> GetCommandBuffers() const { return pCommandBuffers; }

		/**
		 * Get a command buffer from the allocator.
		 * 
		 * @param index The index of the command buffer.
		 * @return The command buffer pointer.
		 */
		const std::shared_ptr<CommandBufferT> GetCommandBuffer(const uint64_t index) const { return pCommandBuffers[index]; }

	protected:
		/**
		 * Flag the command buffer as terminated.
		 * 
		 * @param index The command buffer index.
		 */
		void FlagCommandBufferAsTerminated(uint32_t index) { pCommandBuffers[index]->Terminate(); }

	protected:
		std::vector<std::shared_ptr<CommandBufferT>> pCommandBuffers = {};
		DerivedT* pParentAllocator = nullptr;

		uint32_t m_BufferCount = 0;
	};
}