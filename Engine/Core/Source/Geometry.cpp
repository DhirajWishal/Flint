// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Geometry.hpp"
#include "Core/GeometryStore.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

namespace Flint
{
	uint64_t PipelineIdentifier::hash() const
	{
		const XXH64_hash_t hashes[] = {
			XXH64(&m_VertexDescriptor, sizeof(VertexDescriptor), 0),
			XXH64(m_InstanceTypes.data(), sizeof(DataType) * m_InstanceTypes.size(), 0),
			XXH64(m_TextureTypes.data(), sizeof(TextureType) * m_TextureTypes.size(), 0),
			XXH64(m_ColorTypes.data(), sizeof(ColorType) * m_ColorTypes.size(), 0)
		};

		return static_cast<uint64_t>(XXH64(hashes, sizeof(hashes), 0));
	}

	Mesh::Mesh(Geometry& geometry, VertexDescriptor descriptor, uint64_t vertexCount, uint64_t vertexOffset, uint64_t indexCount, uint64_t indexOffset)
		: m_Geometry(geometry)
		, m_VertexCount(vertexCount), m_VertexOffset(vertexOffset)
		, m_IndexCount(indexCount), m_IndexOffset(indexOffset)
	{
		m_PipelineIdentifier.m_VertexDescriptor = descriptor;
	}

	void Mesh::addMaterial(std::filesystem::path&& texturePath, TextureType type)
	{
		m_TexturePaths.emplace_back(std::move(texturePath));
		m_PipelineIdentifier.m_TextureTypes.emplace_back(type);
	}

	void Mesh::addMaterial(float r, float g, float b, float a, ColorType type)
	{
		m_Colors.emplace_back(std::array<float, 4>{ r, g, b, a });
		m_PipelineIdentifier.m_ColorTypes.emplace_back(type);
	}

	void Mesh::addInstanceType(DataType type)
	{
		m_PipelineIdentifier.m_InstanceTypes.emplace_back(type);
	}

	std::byte* Mesh::mapVertexMemory()
	{
		return m_Geometry.getGeometryStore().mapVertexData(getVertexSize(), getVertexOffset());
	}

	void Mesh::unmapVertexMemory()
	{
		m_Geometry.getGeometryStore().unmapVertexData();
	}

	std::byte* Mesh::mapIndexMemory()
	{
		return m_Geometry.getGeometryStore().mapIndexData(getIndexSize(), getIndexOffset());
	}

	void Mesh::unmapIndexMemory()
	{
		m_Geometry.getGeometryStore().unmapIndexData();
	}

	Geometry::Geometry(GeometryStore& geometryStore, uint64_t vertexSize, uint64_t vertexOffset, uint64_t indexSize, uint64_t indexOffset)
		: m_pGeometryStore(&geometryStore)
		, m_VertexSize(vertexSize), m_VertexOffset(vertexOffset)
		, m_IndexSize(indexSize), m_IndexOffset(indexOffset)
	{
	}

	Mesh& Geometry::createMesh(VertexDescriptor descriptor, uint64_t vertexCount, uint64_t indexCount)
	{
		if (m_Meshes.empty())
			return m_Meshes.emplace_back(*this, descriptor, vertexCount, m_VertexOffset, indexCount, m_IndexOffset);

		const auto& previousMesh = m_Meshes.back();
		return m_Meshes.emplace_back(*this, descriptor, vertexCount, previousMesh.getVertexSize() + previousMesh.getVertexOffset(), indexCount, previousMesh.getIndexSize() + previousMesh.getIndexOffset());
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

	uint8_t VertexDescriptor::getStride() const
	{
		uint8_t stride = 0;

		stride += static_cast<uint8_t>(m_Position);
		stride += static_cast<uint8_t>(m_Normal);

		stride += static_cast<uint8_t>(m_Tangent);
		stride += static_cast<uint8_t>(m_BiTangent);

		stride += static_cast<uint8_t>(m_Color0);
		stride += static_cast<uint8_t>(m_Color1);
		stride += static_cast<uint8_t>(m_Color2);
		stride += static_cast<uint8_t>(m_Color3);
		stride += static_cast<uint8_t>(m_Color4);
		stride += static_cast<uint8_t>(m_Color5);
		stride += static_cast<uint8_t>(m_Color6);
		stride += static_cast<uint8_t>(m_Color7);

		stride += static_cast<uint8_t>(m_Texture0);
		stride += static_cast<uint8_t>(m_Texture1);
		stride += static_cast<uint8_t>(m_Texture2);
		stride += static_cast<uint8_t>(m_Texture3);
		stride += static_cast<uint8_t>(m_Texture4);
		stride += static_cast<uint8_t>(m_Texture5);
		stride += static_cast<uint8_t>(m_Texture6);
		stride += static_cast<uint8_t>(m_Texture7);

		return stride;
	}
}