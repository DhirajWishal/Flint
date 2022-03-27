// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"
#include "Shader.hpp"

namespace Flint
{
	/**
	 * Flint geometry store object.
	 * This object is used to store a number of geometry information, which is then passed to render targets and will be rendered using pipelines.
	 * These stores are organized by vertex attributes.
	 *
	 * The reason to use geometry stores is to increase performance. Switching buffers at draw time is costly. Having a large buffer and then drawing everything from it
	 * is comparatively more beneficial than having multiple small buffers. It also helps us to manage device memory better. The reason to organize these stores according
	 * to vertex attributes is that it allows us to store a large group of similar types data.
	 */
	class GeometryStore final : public DeviceBoundObject, public std::enable_shared_from_this<GeometryStore>
	{
	public:
		/**
		 * Construct the geometry store.
		 *
		 * @param pDevice The device pointer.
		 * @param vertexAttributes The vertex attributes of the geometry.
		 * @param indexSize The size of a single index in bytes.
		 * @param profile The memory profile of the buffer. Default is BufferMemoryProfile::Automatic.
		 */
		GeometryStore(Device* pDevice, const std::vector<ShaderAttribute>& vertexAttributes, uint64_t indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic);
		~GeometryStore() { if (!bIsTerminated) Terminate(); }

		GeometryStore(const GeometryStore&) = delete;
		GeometryStore& operator=(const GeometryStore&) = delete;

		/**
		 * Set data to the geometry store.
		 *
		 * @param pVertexStagingBuffer The stagging buffer containing all the vertex information.
		 * @param pIndexStagingBuffer The stagging buffer containing all the index information.
		 */
		void SetData(const Buffer* pVertexStagingBuffer, const Buffer* pIndexStagingBuffer);

		/**
		 * Set the vertex and index buffers.
		 * This will move in the provided buffers.
		 * 
		 * @param pNewVertexBuffer The vertex buffer pointer.
		 * @param pNewIndexBuffer The index buffer pointer.
		 */
		void SetBuffers(std::unique_ptr<Buffer>&& pNewVertexBuffer, std::unique_ptr<Buffer>&& pNewIndexBuffer);

		/**
		 * Add a geometry to the store.
		 *
		 * @param vertexCount The number of vertexes to add.
		 * @param pVertexData The vertex data pointer.
		 * @param indexCount The number of indexes to add.
		 * @param pIndexData The index data to add.
		 * @return The pair of offsets (vertex offset, index offset) in which the geometry is stored.
		 */
		std::pair<uint64_t, uint64_t> AddGeometry(uint64_t vertexCount, const void* pVertexData, uint64_t indexCount, const void* pIndexData);

		/**
		 * Add geometry to the store using buffers.
		 * This method is preferred over the other adding method.
		 *
		 * @param pVertexStagingBuffer The vertex data stored stagging buffer.
		 * @param pIndexStagingBuffer The index data stored stagging buffer.
		 * @return The pair of offsets (vertex offset, index offset) in which the geometry is stored.
		 */
		std::pair<uint64_t, uint64_t> AddGeometry(const Buffer* pVertexStagingBuffer, const Buffer* pIndexStagingBuffer);

		/**
		 * Remove a geometry from the store.
		 *
		 * @param vertexOffset The vertex offset in the buffer.
		 * @param vertexCount The number of vertexes to remove.
		 * @param indexOffset The index offset in the buffer.
		 * @param indexCount The number of indexes to remove.
		 */
		void RemoveGeometry(uint64_t vertexOffset, uint64_t vertexCount, uint64_t indexOffset, uint64_t indexCount);

	public:
		/**
		 * Terminate the geometry store.
		 */
		virtual void Terminate() override;

	public:
		/**
		 * Get the vertex buffer from the store.
		 *
		 * @return The buffer pointer.
		 */
		Buffer* GetVertexBuffer() const { return pVertexBuffer.get(); }

		/**
		 * Get the index buffer from the store.
		 *
		 * @return The buffer pointer.
		 */
		Buffer* GetIndexBuffer() const { return pIndexBuffer.get(); }

		/**
		 * Get the vertex attributes.
		 *
		 * @return The attribute map.
		 */
		const std::vector<ShaderAttribute> GetVertexAttributes() const { return mVertexAttribtues; }

		/**
		 * Get the size of a single vertex.
		 *
		 * @return The size in bytes.
		 */
		const uint64_t GetVertexSize() const { return mVertexSize; }

		/**
		 * Get the vertex count.
		 *
		 * @return The number of vertexes stored.
		 */
		const uint64_t GetVertexCount() const { return mVertexCount; }

		/**
		 * Get the size of a single index.
		 *
		 * @return The size in bytes.
		 */
		const uint64_t GetIndexSize() const { return mIndexSize; }

		/**
		 * Get the index count.
		 *
		 * @return The number of indexes stored.
		 */
		const uint64_t GetIndexCount() const { return mIndexCount; }

		/**
		 * Map the vertex buffer to the local address space.
		 * Before mapping, make sure that the buffer memory profile is BufferMemoryProfile::TransferFriendly.
		 *
		 * @return The memory address.
		 */
		void* MapVertexBuffer() const;

		/**
		 * Map the index buffer to the local address space.
		 * Before mapping, make sure that the buffer memory profile is BufferMemoryProfile::TransferFriendly.
		 *
		 * @return The memory address.
		 */
		void* MapIndexBuffer() const;

		/**
		 * Unmap the vertex buffer.
		 */
		void UnmapVertexBuffer();

		/**
		 * Unmap the index buffer.
		 */
		void UnmapIndexBuffer();

	private:
		std::vector<ShaderAttribute> mVertexAttribtues = {};

		std::unique_ptr<Buffer> pVertexBuffer = nullptr;
		std::unique_ptr<Buffer> pIndexBuffer = nullptr;

		uint64_t mVertexSize = 0;
		uint64_t mIndexSize = 0;

		uint64_t mVertexCount = 0;
		uint64_t mIndexCount = 0;

		BufferMemoryProfile mMemoryProfile = BufferMemoryProfile::Automatic;
	};
}