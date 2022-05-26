// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Geometry.hpp"
#include "Core/GeometryStore.hpp"

namespace Flint
{
	Mesh::Mesh(std::string&& name, VertexDescriptor descriptor, uint64_t vertexCount, uint64_t vertexOffset, uint64_t indexCount, uint64_t indexOffset)
		: m_Name(std::move(name))
		, m_VertexDescriptor(descriptor)
		, m_VertexCount(vertexCount), m_VertexOffset(vertexOffset)
		, m_IndexCount(indexCount), m_IndexOffset(indexOffset)
	{
	}

	void Mesh::addMaterial(std::filesystem::path&& texturePath, TextureType type)
	{
		m_Textures.emplace_back(type, std::move(texturePath));
	}

	void Mesh::addMaterial(float r, float g, float b, float a, ColorType type)
	{
		m_Colors.emplace_back(type, std::array<float, 4>{ r, g, b, a });
	}

	void Mesh::setInstanceType(InstanceDescriptor&& descriptor)
	{
		m_InstanceDescriptor = std::move(descriptor);
	}

	std::byte* Mesh::mapVertexMemory(Geometry& geometry)
	{
		return geometry.getGeometryStore().mapVertexData(getVertexSize(), getVertexOffset());
	}

	void Mesh::unmapVertexMemory(Geometry& geometry)
	{
		geometry.getGeometryStore().unmapVertexData();
	}

	std::byte* Mesh::mapIndexMemory(Geometry& geometry)
	{
		return geometry.getGeometryStore().mapIndexData(getIndexSize(), getIndexOffset());
	}

	void Mesh::unmapIndexMemory(Geometry& geometry)
	{
		geometry.getGeometryStore().unmapIndexData();
	}

	Geometry::Geometry(GeometryStore& geometryStore, uint64_t vertexSize, uint64_t vertexOffset, uint64_t indexSize, uint64_t indexOffset)
		: m_pGeometryStore(&geometryStore)
		, m_VertexSize(vertexSize), m_VertexOffset(vertexOffset)
		, m_IndexSize(indexSize), m_IndexOffset(indexOffset)
	{
	}

	Mesh& Geometry::createMesh(std::string&& name, VertexDescriptor descriptor, uint64_t vertexCount, uint64_t indexCount)
	{
		if (m_Meshes.empty())
			return m_Meshes.emplace_back(std::move(name), descriptor, vertexCount, m_VertexOffset, indexCount, m_IndexOffset);

		const auto& previousMesh = m_Meshes.back();
		return m_Meshes.emplace_back(std::move(name), descriptor, vertexCount, previousMesh.getVertexSize() + previousMesh.getVertexOffset(), indexCount, previousMesh.getIndexSize() + previousMesh.getIndexOffset());
	}

	std::vector<Flint::MeshMemory> Geometry::beginStreaming()
	{
		std::byte* vertexBegin = m_pGeometryStore->mapVertexData(m_VertexSize, m_VertexOffset);
		std::byte* indexBegin = m_pGeometryStore->mapIndexData(m_IndexSize, m_IndexOffset);

		std::vector<Flint::MeshMemory> meshMemories;
		meshMemories.reserve(m_Meshes.size());

		for (const auto& mesh : m_Meshes)
			meshMemories.emplace_back(vertexBegin, (vertexBegin += mesh.getVertexSize()), indexBegin, (indexBegin + mesh.getIndexSize()));

		return meshMemories;
	}

	void Geometry::endStreaming()
	{
		m_pGeometryStore->unmapVertexData();
		m_pGeometryStore->unmapIndexData();
	}
}