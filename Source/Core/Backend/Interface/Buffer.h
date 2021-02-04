// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Core/Backend/ShaderDigest.h"

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
		FLINT_DEFINE_FUNCTION_POINTER(StaggingBufferHandle, CreateStaggingBuffer, const DeviceHandle& deviceHandle, UI64 size);

		/**
		 * Map the buffer memory to the local address space and return its address.
		 *
		 * @param handle: The stagging buffer handle.
		 * @param size: The size in bytes to be mapped.
		 * @param offset: The offset of the buffer to be mapped.
		 * @return The mapped memory pointer.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void*, MapStaggingBufferMemory, const StaggingBufferHandle& handle, UI64 size, UI64 offset);

		/**
		 * Unmap the mapped memory from the device address space.
		 *
		 * @param handle: The stagging buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, UnmapStaggingBufferMemory, const StaggingBufferHandle& handle);

		/**
		 * Destroy a created buffer.
		 *
		 * @param handle: The stagging buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyStaggingBuffer, const StaggingBufferHandle& handle);

		/**
		 * Create a vertex buffer object.
		 *
		 * @param deviceHandle: The device to which the buffer is bound to.
		 * @param size: The size of the buffer in bytes.
		 * @return The vertex buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(VertexBufferHandle, CreateVertexBuffer, const DeviceHandle& deviceHandle, UI64 size);

		/**
		 * Map the buffer memory to the local address space and return its address.
		 *
		 * @param handle: The vertex buffer handle.
		 * @param size: The size in bytes to be mapped.
		 * @param offset: The offset of the buffer to be mapped.
		 * @return The mapped memory pointer.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void*, MapVertexBufferMemory, const VertexBufferHandle& handle, UI64 size, UI64 offset);

		/**
		 * Unmap the mapped memory from the device address space.
		 *
		 * @param handle: The vertex buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, UnmapVertexBufferMemory, const VertexBufferHandle& handle);

		/**
		 * Destroy a created buffer.
		 *
		 * @param handle: The vertex buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyVertexBuffer, const VertexBufferHandle& handle);

		/**
		 * Create a index buffer object.
		 *
		 * @param deviceHandle: The device to which the buffer is bound to.
		 * @param size: The size of the buffer in bytes.
		 * @return The index buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(IndexBufferHandle, CreateIndexBuffer, const DeviceHandle& deviceHandle, UI64 size);

		/**
		 * Map the buffer memory to the local address space and return its address.
		 *
		 * @param handle: The index buffer handle.
		 * @param size: The size in bytes to be mapped.
		 * @param offset: The offset of the buffer to be mapped.
		 * @return The mapped memory pointer.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void*, MapIndexBufferMemory, const IndexBufferHandle& handle, UI64 size, UI64 offset);

		/**
		 * Unmap the mapped memory from the device address space.
		 *
		 * @param handle: The index buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, UnmapIndexBufferMemory, const IndexBufferHandle& handle);

		/**
		 * Destroy a created buffer.
		 *
		 * @param handle: The index buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyIndexBuffer, const IndexBufferHandle& handle);

		/**
		 * Create a uniform buffer object.
		 *
		 * @param deviceHandle: The device to which the buffer is bound to.
		 * @param size: The size of the buffer in bytes.
		 * @return The uniform buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(UniformBufferHandle, CreateUniformBuffer, const DeviceHandle& deviceHandle, UI64 size);

		/**
		 * Map the buffer memory to the local address space and return its address.
		 *
		 * @param handle: The uniform buffer handle.
		 * @param size: The size in bytes to be mapped.
		 * @param offset: The offset of the buffer to be mapped.
		 * @return The mapped memory pointer.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void*, MapUniformBufferMemory, const UniformBufferHandle& handle, UI64 size, UI64 offset);

		/**
		 * Unmap the mapped memory from the device address space.
		 *
		 * @param handle: The uniform buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, UnmapUniformBufferMemory, const UniformBufferHandle& handle);

		/**
		 * Destroy a created buffer.
		 *
		 * @param handle: The uniform buffer handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyUniformBuffer, const UniformBufferHandle& handle);
	}
}