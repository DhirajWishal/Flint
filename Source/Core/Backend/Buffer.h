// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"

namespace Flint
{
	/**
	 * Buffer usage enum.
	 * Defines what the buffer will be used for,
	 */
	enum class BufferUsage : UI8 {
		UNDEFINED,
		VERTEX, INDEX, STAGGING, UNIFORM
	};

	/**
	 * Memory profile enum.
	 * Defines what the buffer memory should look like. This can be used to optimize performance.
	 */
	enum class MemoryProfile : UI8 {
		/**
		 * This type is a must if the buffer's content is being copied at any moment.
		 * Try not to set this for buffer types like Vertex and Index as it might be costly.
		 */
		TRANSFER_FRIENDLY,

		/**
		 * This is the best for speed.
		 * Buffers with this profile has the fastest access time. Usually this is set for draw-based buffers like Vertex and Index buffers.
		 * Unlike DRAW_RESOURCE type, this has a slight performance cost.
		 */
		 DEVICE_ONLY,

		 /**
		  * This is used for buffer types like Vertex and Index buffers and is optimized for speed.
		  */
		  DRAW_RESOURCE
	};

	/**
	 * Buffer object.
	 * This object is the base class for all the backend buffer objects.
	 */
	class FBuffer : public BackendObject {
		struct PreviousMemoryMapInfo {
			UI64 mSize = 0;
			UI64 mOffset = 0;
		};

	public:
		FBuffer() {}
		virtual ~FBuffer() {}

		/**
		 * Initialize the buffer.
		 *
		 * @param pDevice: The device pointer.
		 * @param size: The size of the buffer.
		 * @param usage: The buffer usage.
		 * @param profile: The buffer's memory profile.
		 */
		virtual void Initialize(FDevice* pDevice, UI64 size, BufferUsage usage, MemoryProfile profile) = 0;

		/**
		 * Terminate the buffer.
		 */
		virtual void Terminate() = 0;

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
		 * Get the device object the buffer is bound to.
		 *
		 * @return The device pointer.
		 */
		FDevice* GetDevice() const { return pDevice; }

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

	protected:
		PreviousMemoryMapInfo mPrevMapInfo = {};

		FDevice* pDevice = {};
		UI64 mSize = 0;

		BufferUsage mUsage = BufferUsage::UNDEFINED;
		MemoryProfile mMemoryProfile = MemoryProfile::TRANSFER_FRIENDLY;
	};
}