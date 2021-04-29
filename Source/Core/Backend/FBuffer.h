// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDevice.h"

namespace Flint
{
	/**
	 * Buffer object.
	 * This object is the base class for all the backend buffer objects.
	 */
	class FBuffer : public BackendObject, public std::enable_shared_from_this<FBuffer> {
		struct PreviousMemoryMapInfo {
			UI64 mSize = 0;
			UI64 mOffset = 0;
		};

	public:
		FBuffer(std::shared_ptr<FDevice> pDevice, UI64 size, BufferUsage usage, MemoryProfile profile) : pDevice(pDevice), mSize(size), mUsage(usage), mMemoryProfile(profile) {}
		virtual ~FBuffer() {}

		/**
		 * Map the buffer memory to the local address space.
		 * Note that the size + offset must not exceed the buffer size.
		 *
		 * @param size: The size to be mapped.
		 * @param offset: The offset of the buffer to be mapped.
		 */
		virtual void* MapMemory(UI64 size, UI64 offset) = 0;

		/**
		 * Flush the mapped memory.
		 * We recommend this as it guarantees that the copied content will be pushed to the device.
		 */
		virtual void FlushMemoryMappings() = 0;

		/**
		 * Unmap the mapped memory from the local address space.
		 */
		virtual void UnmapMemory() = 0;

		/**
		 * Copy buffer data from another buffer.
		 *
		 * @param pBuffer: The other buffer to copy data from.
		 * @param size: The size to be copied.
		 * @param srcOffset: The source buffer's offset.
		 * @param dstOffset: The destination buffer's (this) offset.
		 */
		virtual void CopyFrom(const FBuffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) = 0;

		/**
		 * Get the size of the buffer.
		 *
		 * @return The size in bytes.
		 */
		UI64 GetSize() const { return mSize; }

		/**
		 * Get the buffer usage.
		 *
		 * @return The usage.
		 */
		BufferUsage GetUsage() const { return mUsage; }

		/**
		 * Get the memory profile of the buffer.
		 *
		 * @return The memory profile.
		 */
		MemoryProfile GetMemoryProfile() const { return mMemoryProfile; }

		/**
		 * Get the device which the buffer is bound to.
		 * 
		 * @return The device pointer.
		 */
		std::shared_ptr<FDevice> GetDevice() const { return pDevice; }

	protected:
		PreviousMemoryMapInfo mPrevMapInfo = {};

		std::shared_ptr<FDevice> pDevice = nullptr;
		UI64 mSize = 0;

		BufferUsage mUsage = BufferUsage::UNDEFINED;
		MemoryProfile mMemoryProfile = MemoryProfile::TRANSFER_FRIENDLY;
	};
}