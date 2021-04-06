// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDevice.h"

namespace Flint
{
	/**
	 * Flint buffer object.
	 * This object is used to store data in a device (GPU).
	 */
	class FBuffer final : public FObject {
	public:
		FBuffer();
		explicit FBuffer(Backend::BackendObject* pBackendObject);
		~FBuffer();

		/**
		 * Initialize the buffer.
		 * 
		 * @param device: The device object.
		 * @param size: The size of the buffer.
		 * @param usage: The buffer usage.
		 * @param profile: The buffer's memory profile.
		 */
		void Initialize(const FDevice& device, UI64 size, Backend::BufferUsage usage, Backend::MemoryProfile profile);

		/**
		 * Terminate the buffer.
		 */
		void Terminate();

		/**
		 * Map the buffer's address space to the local address space.
		 * 
		 * @param size: The size of the mapping. The size must be { 0 < x <= std::numeric_limits<UI64>::max() } where by specifying the upper bound, the full buffer is mapped.
		 * @param offset: The offset of the buffer memory.
		 */
		void* MapMemory(UI64 size, UI64 offset);

		/**
		 * Flush the memory mapping.
		 * Flushing must be done prior to unmapping as sometimes just unmapping does not submit the data to the GPU.
		 */
		void FlushMemoryMapping();

		/**
		 * Unmap the mapped memory.
		 */
		void UnmapMemory();

		/**
		 * Copy buffer data from another buffer.
		 * 
		 * @param other: The buffer to copy data from.
		 * @param size: The size of the copy.
		 * @param srcOffset: The offset in source buffer to copy data to.
		 * @param dstOffset: The destination (this) buffer's offset.
		 */
		void CopyFrom(const FBuffer& other, UI64 size, UI64 srcOffset, UI64 dstOffset);

	public:
		FDevice GetDevice() const;
		UI64 GetSize() const;
	};

	typedef std::unordered_map<String, FBuffer> FUniformBufferContainer;
}