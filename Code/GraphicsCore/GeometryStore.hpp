// Copyright 2021 Dhiraj Wishal
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
		 * @param pDevice: The device pointer.
		 * @param vertexAttributes: The vertex attributes of the geometry.
		 * @param indexSize: The size of a single index in bytes.
		 * @param profile: The memory profile of the buffer. Default is BufferMemoryProfile::Automatic.
		 */
		GeometryStore(const std::shared_ptr<Device>& pDevice, const TShaderAttributeMap& vertexAttributes, UI64 indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic);
		~GeometryStore() { if (!bIsTerminated) Terminate(); }

		GeometryStore(const GeometryStore&) = delete;
		GeometryStore& operator=(const GeometryStore&) = delete;

		/**
		 * Set data to the geometry store.
		 *
		 * @param pVertexStagingBuffer: The stagging buffer containing all the vertex information.
		 * @param pIndexStagingBuffer: The stagging buffer containing all the index information.
		 */
		void SetData(const std::shared_ptr<Buffer>& pVertexStagingBuffer, const std::shared_ptr<Buffer>& pIndexStagingBuffer);

		/**
		 * Add a geometry to the store.
		 *
		 * @param vertexCount: The number of vertexes to add.
		 * @param pVertexData: The vertex data pointer.
		 * @param indexCount: The number of indexes to add.
		 * @param pIndexData: The index data to add.
		 * @return The pair of offsets (vertex offset, index offset) in which the geometry is stored.
		 */
		std::pair<UI64, UI64> AddGeometry(UI64 vertexCount, const void* pVertexData, UI64 indexCount, const void* pIndexData);

		/**
		 * Add geometry to the store using buffers.
		 * This method is preferred over the other adding method.
		 *
		 * @param pVertexStagingBuffer: The vertex data stored stagging buffer.
		 * @param pIndexStagingBuffer: The index data stored stagging buffer.
		 * @return The pair of offsets (vertex offset, index offset) in which the geometry is stored.
		 */
		std::pair<UI64, UI64> AddGeometry(const std::shared_ptr<Buffer>& pVertexStagingBuffer, const std::shared_ptr<Buffer>& pIndexStagingBuffer);

		/**
		 * Remove a geometry from the store.
		 *
		 * @param vertexOffset: The vertex offset in the buffer.
		 * @param vertexCount: The number of vertexes to remove.
		 * @param indexOffset: The index offset in the buffer.
		 * @param indexCount: The number of indexes to remove.
		 */
		void RemoveGeometry(UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount);

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
		const std::shared_ptr<Buffer> GetVertexBuffer() const { return pVertexBuffer; }

		/**
		 * Get the index buffer from the store.
		 *
		 * @return The buffer pointer.
		 */
		const std::shared_ptr<Buffer> GetIndexBuffer() const { return pIndexBuffer; }

		/**
		 * Get the vertex attributes.
		 *
		 * @return The attribute map.
		 */
		const TShaderAttributeMap GetTheVertexAttributes() const { return mVertexAttribtues; }

		/**
		 * Get the size of a single vertex.
		 *
		 * @return The size in bytes.
		 */
		const UI64 GetVertexSize() const { return mVertexSize; }

		/**
		 * Get the vertex count.
		 *
		 * @return The number of vertexes stored.
		 */
		const UI64 GetVertexCount() const { return mVertexCount; }

		/**
		 * Get the size of a single index.
		 *
		 * @return The size in bytes.
		 */
		const UI64 GetIndexSize() const { return mIndexSize; }

		/**
		 * Get the index count.
		 *
		 * @return The number of indexes stored.
		 */
		const UI64 GetIndexCount() const { return mIndexCount; }

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
		TShaderAttributeMap mVertexAttribtues = {};

		std::shared_ptr<Buffer> pVertexBuffer = nullptr;
		std::shared_ptr<Buffer> pIndexBuffer = nullptr;

		UI64 mVertexSize = 0;
		UI64 mIndexSize = 0;

		UI64 mVertexCount = 0;
		UI64 mIndexCount = 0;

		BufferMemoryProfile mMemoryProfile = BufferMemoryProfile::Automatic;
	};
}