// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"

namespace Flint
{
	namespace Interface
	{
		/**
		 * Create a stagging buffer object.
		 *
		 * @param deviceHandle: The device to which the buffer is bound to.
		 * @param size: The size of the buffer in bytes.
		 * @return The stagging buffer handle.
		 */
		inline StaggingBufferHandle(*CreateStaggingBuffer)(const DeviceHandle& deviceHandle, UI64 size) = nullptr;

		/**
		 * Map the buffer memory to the local address space and return its address.
		 *
		 * @param handle: The stagging buffer handle.
		 * @param size: The size in bytes to be mapped.
		 * @param offset: The offset of the buffer to be mapped.
		 * @return The mapped memory pointer.
		 */
		inline void* (*MapStaggingBufferMemory)(const StaggingBufferHandle& handle, UI64 size, UI64 offset) = nullptr;

		/**
		 * Unmap the mapped memory from the device address space.
		 *
		 * @param handle: The stagging buffer handle.
		 */
		inline void (*UnmapStaggingBufferMemory)(const StaggingBufferHandle& handle) = nullptr;

		/**
		 * Destroy a created buffer.
		 *
		 * @param handle: The stagging buffer handle.
		 */
		inline void (*DestroyStaggingBuffer)(const StaggingBufferHandle& handle) = nullptr;

		/**
		 * Create a vertex buffer object.
		 *
		 * @param deviceHandle: The device to which the buffer is bound to.
		 * @param size: The size of the buffer in bytes.
		 * @return The vertex buffer handle.
		 */
		inline VertexBufferHandle(*CreateVertexBuffer)(const DeviceHandle& deviceHandle, UI64 size) = nullptr;

		/**
		 * Map the buffer memory to the local address space and return its address.
		 *
		 * @param handle: The vertex buffer handle.
		 * @param size: The size in bytes to be mapped.
		 * @param offset: The offset of the buffer to be mapped.
		 * @return The mapped memory pointer.
		 */
		inline void* (*MapVertexBufferMemory)(const VertexBufferHandle& handle, UI64 size, UI64 offset) = nullptr;

		/**
		 * Unmap the mapped memory from the device address space.
		 *
		 * @param handle: The vertex buffer handle.
		 */
		inline void (*UnmapVertexBufferMemory)(const VertexBufferHandle& handle) = nullptr;

		/**
		 * Destroy a created buffer.
		 *
		 * @param handle: The vertex buffer handle.
		 */
		inline void (*DestroyVertexBuffer)(const VertexBufferHandle& handle) = nullptr;

		/**
		 * Create a index buffer object.
		 *
		 * @param deviceHandle: The device to which the buffer is bound to.
		 * @param size: The size of the buffer in bytes.
		 * @return The index buffer handle.
		 */
		inline IndexBufferHandle(*CreateIndexBuffer)(const DeviceHandle& deviceHandle, UI64 size) = nullptr;

		/**
		 * Map the buffer memory to the local address space and return its address.
		 *
		 * @param handle: The index buffer handle.
		 * @param size: The size in bytes to be mapped.
		 * @param offset: The offset of the buffer to be mapped.
		 * @return The mapped memory pointer.
		 */
		inline void* (*MapIndexBufferMemory)(const IndexBufferHandle& handle, UI64 size, UI64 offset) = nullptr;

		/**
		 * Unmap the mapped memory from the device address space.
		 *
		 * @param handle: The index buffer handle.
		 */
		inline void (*UnmapIndexBufferMemory)(const IndexBufferHandle& handle) = nullptr;

		/**
		 * Destroy a created buffer.
		 *
		 * @param handle: The index buffer handle.
		 */
		inline void (*DestroyIndexBuffer)(const IndexBufferHandle& handle) = nullptr;

		/**
		 * Create a uniform buffer object.
		 *
		 * @param deviceHandle: The device to which the buffer is bound to.
		 * @param size: The size of the buffer in bytes.
		 * @return The uniform buffer handle.
		 */
		inline UniformBufferHandle(*CreateUniformBuffer)(const DeviceHandle& deviceHandle, UI64 size) = nullptr;

		/**
		 * Map the buffer memory to the local address space and return its address.
		 *
		 * @param handle: The uniform buffer handle.
		 * @param size: The size in bytes to be mapped.
		 * @param offset: The offset of the buffer to be mapped.
		 * @return The mapped memory pointer.
		 */
		inline void* (*MapUniformBufferMemory)(const UniformBufferHandle& handle, UI64 size, UI64 offset) = nullptr;

		/**
		 * Unmap the mapped memory from the device address space.
		 *
		 * @param handle: The uniform buffer handle.
		 */
		inline void (*UnmapUniformBufferMemory)(const UniformBufferHandle& handle) = nullptr;

		/**
		 * Destroy a created buffer.
		 *
		 * @param handle: The uniform buffer handle.
		 */
		inline void (*DestroyUniformBuffer)(const UniformBufferHandle& handle) = nullptr;
	}
}