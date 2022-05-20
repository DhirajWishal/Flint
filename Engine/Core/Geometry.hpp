// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "EngineBoundObject.hpp"

namespace Flint
{
	class Geometry;
	class GeometryStore;

	/**
	 * Resource buffer view class.
	 * This contains the begin and end pointers of the buffer's data.
	 */
	class ResourceBufferView final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pFirst The first/ begin pointer.
		 * @param pLast The last/ end pointer.
		 */
		explicit ResourceBufferView(std::byte* pFirst, std::byte* pLast) : m_pBegin(pFirst), m_pEnd(pLast) {}

		/**
		 * Get the begin pointer.
		 *
		 * @return The pointer.
		 */
		[[nodiscard]] std::byte* begin() { return m_pBegin; }

		/**
		 * Get the end pointer.
		 *
		 * @return The pointer.
		 */
		[[nodiscard]] std::byte* end() { return m_pEnd; }

		/**
		 * Get the begin pointer.
		 *
		 * @return The pointer.
		 */
		[[nodiscard]] const std::byte* begin() const { return m_pBegin; }

		/**
		 * Get the end pointer.
		 *
		 * @return The pointer.
		 */
		[[nodiscard]] const std::byte* end() const { return m_pEnd; }

		/**
		 * Get the constant begin pointer.
		 *
		 * @return The pointer.
		 */
		[[nodiscard]] const std::byte* cbegin() const { return m_pBegin; }

		/**
		 * Get the constant end pointer.
		 *
		 * @return The pointer.
		 */
		[[nodiscard]] const std::byte* cend() const { return m_pEnd; }

	private:
		std::byte* m_pBegin = nullptr;
		std::byte* m_pEnd = nullptr;
	};

	/**
	 * Mesh memory class.
	 * This structure can be used to load data to the mesh.
	 */
	class MeshMemory final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pVertexFirst The first/ begin pointer of the vertex buffer.
		 * @param pVertexLast The last/ end pointer of the vertex buffer.
		 * @param pIndexFirst The first/ begin pointer of the index buffer.
		 * @param pIndexLast The last/ end pointer of the index buffer.
		 */
		explicit MeshMemory(std::byte* pVertexFirst, std::byte* pVertexLast, std::byte* pIndexFirst, std::byte* pIndexLast)
			: m_VertexBufferView(pVertexFirst, pVertexLast), m_IndexBufferView(pIndexFirst, pIndexLast) {}

		/**
		 * Get the vertex buffer view.
		 *
		 * @reutrn The buffer view.
		 */
		[[nodiscard]] ResourceBufferView getVertexBufferView() const { return m_VertexBufferView; }

		/**
		 * Get the index buffer view.
		 *
		 * @reutrn The buffer view.
		 */
		[[nodiscard]] ResourceBufferView getIndexBufferView() const { return m_IndexBufferView; }

	private:
		ResourceBufferView m_VertexBufferView;
		ResourceBufferView m_IndexBufferView;
	};

	/**
	 * Mesh class.
	 * This contains the information about a single mesh in the geometry.
	 */
	class Mesh final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param geometry The geometry to which this mesh belongs to.
		 * @param descriptor The vertex descriptor.
		 * @param vertexCount The number of vertices to store.
		 * @param vertexOffset The vertex offset.
		 * @param indexCount The number of indices to store.
		 * @param indexOffset The offset of the indices.
		 */
		explicit Mesh(Geometry& geometry, VertexDescriptor descriptor, uint64_t vertexCount, uint64_t vertexOffset, uint64_t indexCount, uint64_t indexOffset);

		/**
		 * Map the vertex memory to the local address space.
		 *
		 * @return The vertex memory.
		 */
		[[nodiscard]] std::byte* mapVertexMemory();

		/**
		 * Unmap the mapped vertex memory.
		 */
		void unmapVertexMemory();

		/**
		 * Map the index memory to the local address space.
		 *
		 * @return The index memory.
		 */
		[[nodiscard]] std::byte* mapIndexMemory();

		/**
		 * Unmap the mapped index memory.
		 */
		void unmapIndexMemory();

		/**
		 * Get the geometry to which this mesh belongs to.
		 *
		 * @return The geometry.
		 */
		[[nodiscard]] Geometry& getGeometry() { return m_Geometry; }

		/**
		 * Get the geometry to which this mesh belongs to.
		 *
		 * @return The geometry.
		 */
		[[nodiscard]] const Geometry& getGeometry() const { return m_Geometry; }

		/**
		 * Get the vertex descriptor.
		 *
		 * @return The descriptor.
		 */
		[[nodiscard]] VertexDescriptor getVertexDescriptor() const { return m_VertexDescriptor; }

		/**
		 * Get the vertex stride.
		 *
		 * @return The stride.
		 */
		[[nodiscard]] uint64_t getVertexStride() const { return m_VertexDescriptor.getStride(); }

		/**
		 * Get the vertex count.
		 *
		 * @return The count.
		 */
		[[nodiscard]] uint64_t getVertexCount() const { return m_VertexCount; }

		/**
		 * Get the vertex offset.
		 *
		 * @return The offset.
		 */
		[[nodiscard]] uint64_t getVertexOffset() const { return m_VertexOffset; }

		/**
		 * Get the total vertex size.
		 *
		 * @return The size in bytes.
		 */
		[[nodiscard]] uint64_t getVertexSize() const { return m_VertexCount * m_VertexDescriptor.getStride(); }

		/**
		 * Get the index count.
		 *
		 * @return The count.
		 */
		[[nodiscard]] uint64_t getIndexCount() const { return m_IndexCount; }

		/**
		 * Get the index offset.
		 *
		 * @return The offset.
		 */
		[[nodiscard]] uint64_t getIndexOffset() const { return m_IndexOffset; }

		/**
		 * Get the total index size.
		 *
		 * @return The size in bytes.
		 */
		[[nodiscard]] uint64_t getIndexSize() const { return m_IndexCount * sizeof(uint32_t); }

	private:
		std::string m_Name;

		Geometry& m_Geometry;

		uint64_t m_VertexCount = 0;
		uint64_t m_VertexOffset = 0;

		uint64_t m_IndexCount = 0;
		uint64_t m_IndexOffset = 0;

		VertexDescriptor m_VertexDescriptor;
	};

	/**
	 * Geometry class.
	 * This object stores information about a single geometry.
	 */
	class Geometry final
	{
	public:
		/**
		 * Default constructor.
		 */
		constexpr Geometry() = default;

		/**
		 * Explicit constructor.
		 *
		 * @param geometryStore The geometry store to which this object is bound to.
		 * @param vertexSize The vertex size.
		 * @param vertexOffset The vertex offset.
		 * @parm indexSize The index size.
		 * @parm indexOffset The index offset.
		 */
		explicit Geometry(GeometryStore& geometryStore, uint64_t vertexSize, uint64_t vertexOffset, uint64_t indexSize, uint64_t indexOffset);

		/**
		 * Create a new mesh.
		 *
		 * @param descriptor The vertex descriptor.
		 * @param vertexCount The number of vertices to store.
		 * @param indexCount The number of indices to store.
		 */
		[[nodiscard]] Mesh& createMesh(VertexDescriptor descriptor, uint64_t vertexCount, uint64_t indexCount);

		/**
		 * Get the geometry store to which this object is bound to.
		 * Make sure that the object is valid.
		 *
		 * @return The geometry store reference.
		 */
		[[nodiscard]] GeometryStore& getGeometryStore() { return *m_pGeometryStore; }

		/**
		 * Get the geometry store to which this object is bound to.
		 * Make sure that the object is valid.
		 *
		 * @return The geometry store reference.
		 */
		[[nodiscard]] const GeometryStore& getGeometryStore() const { return *m_pGeometryStore; }

		/**
		 * Begin data streaming.
		 * Using this, you can access the individual mesh data from the device.
		 *
		 * @return The mesh memory. Each memory corresponds to the actual mesh in the mesh list.
		 */
		[[nodiscard]] std::vector<MeshMemory> beginStreaming();

		/**
		 * End streaming.
		 * This will send the data to the GPU.
		 */
		void endStreaming();

	private:
		std::vector<Mesh> m_Meshes;

		GeometryStore* m_pGeometryStore = nullptr;

		uint64_t m_VertexSize = 0;
		uint64_t m_VertexOffset = 0;
		uint64_t m_IndexSize = 0;
		uint64_t m_IndexOffset = 0;
	};
}