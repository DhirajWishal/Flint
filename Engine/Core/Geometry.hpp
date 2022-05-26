// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "EngineBoundObject.hpp"

#include <array>

namespace Flint
{
	class Geometry;
	class GeometryStore;

	/**
	 * Texture type enum.
	 */
	enum class TextureType : uint8_t
	{
		BaseColor,
		NormalCamera,
		EmissionColor,
		Metalness,
		DiffuseRoughness,
		AmbientOcclusion,
		SheenColor,
		SheenRoughness,
		ClearCoat,
		ClearCoatRoughness,
		ClearCoatNormal,
		Transmission,

		Unknown
	};

	/**
	 * Color type enum.
	 */
	enum class ColorType : uint8_t
	{
		Diffuse,
		Ambient,
		Specular,
		Emissive,
		Transparent,
		Reflective,
		Base
	};

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
		 * @param name The name of the mesh.
		 * @param descriptor The vertex descriptor.
		 * @param vertexCount The number of vertices to store.
		 * @param vertexOffset The vertex offset.
		 * @param indexCount The number of indices to store.
		 * @param indexOffset The offset of the indices.
		 */
		explicit Mesh(std::string&& name, VertexDescriptor descriptor, uint64_t vertexCount, uint64_t vertexOffset, uint64_t indexCount, uint64_t indexOffset);

		/**
		 * Add a texture material to the mesh.
		 *
		 * @param texturePath The path to the texture material to add.
		 * @param type The texture type.
		 */
		void addMaterial(std::filesystem::path&& texturePath, TextureType type);

		/**
		 * Add a color material to the mesh.
		 *
		 * @param r The red color.
		 * @param g The green color.
		 * @param b The blue color.
		 * @param a The alpha color.
		 * @param type The color type.
		 */
		void addMaterial(float r, float g, float b, float a, ColorType type);

		/**
		 * Set the instance descriptor to the mesh.
		 *
		 * @param descriptor The instance descriptor..
		 */
		void setInstanceType(InstanceDescriptor&& descriptor);

		/**
		 * Get the texture paths.
		 *
		 * @return The paths.
		 */
		[[nodiscard]] const std::vector<std::pair<TextureType, std::filesystem::path>>& getTextures() const noexcept { return m_Textures; }

		/**
		 * Get the color materials.
		 *
		 * @return The colors.
		 */
		[[nodiscard]] const std::vector<std::pair<ColorType, std::array<float, 4>>>& getColors() const noexcept { return m_Colors; }

		/**
		 * Map the vertex memory to the local address space.
		 *
		 * @param The geometry to which the mesh is bound to.
		 * @return The vertex memory.
		 */
		[[nodiscard]] std::byte* mapVertexMemory(Geometry& geometry);

		/**
		 * Unmap the mapped vertex memory.
		 *
		 * @param The geometry to which the mesh is bound to.
		 */
		void unmapVertexMemory(Geometry& geometry);

		/**
		 * Map the index memory to the local address space.
		 *
		 * @param The geometry to which the mesh is bound to.
		 * @return The index memory.
		 */
		[[nodiscard]] std::byte* mapIndexMemory(Geometry& geometry);

		/**
		 * Unmap the mapped index memory.
		 *
		 * @param The geometry to which the mesh is bound to.
		 */
		void unmapIndexMemory(Geometry& geometry);

		/**
		 * Get the name of this mesh.
		 *
		 * @return The mesh name.
		 */
		[[nodiscard]] std::string_view getName() const noexcept { return m_Name; }

		/**
		 * Get the vertex descriptor.
		 *
		 * @return The descriptor.
		 */
		[[nodiscard]] VertexDescriptor getVertexDescriptor() const noexcept { return m_VertexDescriptor; }

		/**
		 * Get the instance descriptor.
		 *
		 * @return The descriptor.
		 */
		[[nodiscard]] InstanceDescriptor getInstanceDescriptor() const noexcept { return m_InstanceDescriptor; }

		/**
		 * Get the vertex stride.
		 *
		 * @return The stride.
		 */
		[[nodiscard]] uint64_t getVertexStride() const noexcept { return GetStride(m_VertexDescriptor); }

		/**
		 * Get the vertex count.
		 *
		 * @return The count.
		 */
		[[nodiscard]] uint64_t getVertexCount() const noexcept { return m_VertexCount; }

		/**
		 * Get the vertex offset.
		 *
		 * @return The offset.
		 */
		[[nodiscard]] uint64_t getVertexOffset() const noexcept { return m_VertexOffset; }

		/**
		 * Get the total vertex size.
		 *
		 * @return The size in bytes.
		 */
		[[nodiscard]] uint64_t getVertexSize() const noexcept { return m_VertexCount * GetStride(m_VertexDescriptor); }

		/**
		 * Get the index count.
		 *
		 * @return The count.
		 */
		[[nodiscard]] uint64_t getIndexCount() const noexcept { return m_IndexCount; }

		/**
		 * Get the index offset.
		 *
		 * @return The offset.
		 */
		[[nodiscard]] uint64_t getIndexOffset() const noexcept { return m_IndexOffset; }

		/**
		 * Get the total index size.
		 *
		 * @return The size in bytes.
		 */
		[[nodiscard]] uint64_t getIndexSize() const noexcept { return m_IndexCount * sizeof(uint32_t); }

	private:
		VertexDescriptor m_VertexDescriptor;
		InstanceDescriptor m_InstanceDescriptor;

		std::vector<std::pair<TextureType, std::filesystem::path>> m_Textures;
		std::vector<std::pair<ColorType, std::array<float, 4>>> m_Colors;

		std::vector<DataType> m_InstanceTypes;

		std::string m_Name;

		uint64_t m_VertexCount = 0;
		uint64_t m_VertexOffset = 0;

		uint64_t m_IndexCount = 0;
		uint64_t m_IndexOffset = 0;
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
		 * @param name The name of the mesh.
		 * @param descriptor The vertex descriptor.
		 * @param vertexCount The number of vertices to store.
		 * @param indexCount The number of indices to store.
		 */
		[[nodiscard]] Mesh& createMesh(std::string&& name, VertexDescriptor descriptor, uint64_t vertexCount, uint64_t indexCount);

		/**
		 * Get the geometry store to which this object is bound to.
		 * Make sure that the object is valid.
		 *
		 * @return The geometry store reference.
		 */
		[[nodiscard]] GeometryStore& getGeometryStore() noexcept { return *m_pGeometryStore; }

		/**
		 * Get the geometry store to which this object is bound to.
		 * Make sure that the object is valid.
		 *
		 * @return The geometry store reference.
		 */
		[[nodiscard]] const GeometryStore& getGeometryStore() const noexcept { return *m_pGeometryStore; }

		/**
		 * Get all the stored meshes.
		 *
		 * @return The meshes.
		 */
		[[nodiscard]] const std::vector<Mesh>& getMeshes() const noexcept { return m_Meshes; }

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