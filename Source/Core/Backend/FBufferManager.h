// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDeviceManager.h"

namespace Flint
{
	class FBufferManager;

	/**
	 * Buffer usage enum.
	 */
	enum class BufferUsage : UI8 {
		/**
		 * Undefined buffer type. This will result in an error.
		 */
		UNDEFINED,

		/**
		 * Stagging buffers are used to transfer data between buffers and images.
		 */
		STAGGING,

		/**
		 * Vertex Buffers are used to submit vertex data to the pipeline.
		 */
		VERTEX,

		/**
		 * Index buffers are used to submit index data to the pipeline.
		 */
		INDEX,

		/**
		 * Uniform buffers are used to submit uniform data to the pipeline.
		 */
		UNIFORM,

		/**
		 * Storage buffers are used to store processed or pre-processed data in the pipeline.
		 */
		STORAGE,
	};

	/**
	 * Memory usage enum.
	 */
	enum class MemoryProfile : UI8 {
		/**
		 * This memory profile is best suited for transferring data from the client application to the buffer.
		 */
		TRANSFER_FRIENDLY,

		/**
		 * This memory profile is best suited for device only uses.
		 */
		 DEVICE_ONLY,

		/**
		 * This memory profile is best suited for Vertex and Index buffers.
		 */
		DRAW_RESOURCE,
	};

	/**
	 * Flint buffer handle.
	 */
	struct FBufferHandle
	{
		FBufferHandle() = default;
		FBufferHandle(FBufferManager* pManager, UI32 index) : pManager(pManager), mIndex(index) {}

		operator const UI32() const { return mIndex; }

	private:
		FBufferManager* pManager = nullptr;
		UI32 mIndex = 0;
	};

	/**
	 * Flint buffer manager object.
	 * This object manages all the aspects of buffers.
	 */
	class FBufferManager
	{
	public:
		FBufferManager() = default;

		/**
		 * Create a new buffer object and return its handle.
		 *
		 * @param deviceHandle: The device handle to which the buffer is bound to.
		 * @param size: The size of the buffer.
		 * @param usage: The buffer usage.
		 * @param memoryProfile: The memory profile of the buffer.
		 * @return The created buffer handle.
		 */
		virtual FBufferHandle CreatBuffer(const FDeviceHandle& deviceHandle, UI64 size, BufferUsage usage, MemoryProfile memoryProfile) = 0;

		/**
		 * Map buffer's content to the local address space.
		 *
		 * @param buffer: The buffer handle.
		 * @param size: The size to be mapped.
		 * @param offset: The offset of the buffer to be mapped.
		 * @return The mapped memory range.
		 */
		virtual void* MapBufferMemory(const FBufferHandle& buffer, UI64 size, UI64 offset) = 0;

		/**
		 * Unmap the buffer from the local address space.
		 *
		 * @param buffer: The buffer handle.
		 */
		virtual void UnmapBufferMemory(const FBufferHandle& buffer) = 0;

		/**
		 * Destroy a created buffer object.
		 *
		 * @param handle: The buffer handle.
		 */
		virtual void Terminate(FBufferHandle& handle) = 0;
	};
}