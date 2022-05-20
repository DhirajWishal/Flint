// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/StaticEntity.hpp"
#include "Core/GeometryStore.hpp"
#include "Core/Errors/AssetError.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <optick.h>

#include <execution>

namespace /* anonymous */
{
	/**
	 * Get the basic mesh information.
	 *
	 * @param pMesh The mesh pointer.
	 * @return The descriptor, vertex count and index count.
	 */
	std::tuple<Flint::VertexDescriptor, uint64_t, uint64_t> GetBasicMeshData(const aiMesh* pMesh)
	{
		Flint::VertexDescriptor descriptor;
		uint64_t vertices = 0, indices = 0;

		// Get the vertex information.
		if (pMesh->HasNormals())
			descriptor.m_Normal = Flint::PositionType::X32Y32Z32;

		if (pMesh->HasPositions())
			descriptor.m_Position = Flint::PositionType::X32Y32Z32;

		if (pMesh->HasTangentsAndBitangents())
		{
			descriptor.m_Tangent = Flint::PositionType::X32Y32Z32;
			descriptor.m_BiTangent = Flint::PositionType::X32Y32Z32;
		}

		// Resolve vertex colors.
		if (pMesh->HasVertexColors(0)) descriptor.m_Color0 = Flint::CoordinateType::R8G8B8;
		if (pMesh->HasVertexColors(1)) descriptor.m_Color1 = Flint::CoordinateType::R8G8B8;
		if (pMesh->HasVertexColors(2)) descriptor.m_Color2 = Flint::CoordinateType::R8G8B8;
		if (pMesh->HasVertexColors(3)) descriptor.m_Color3 = Flint::CoordinateType::R8G8B8;
		if (pMesh->HasVertexColors(4)) descriptor.m_Color4 = Flint::CoordinateType::R8G8B8;
		if (pMesh->HasVertexColors(5)) descriptor.m_Color5 = Flint::CoordinateType::R8G8B8;
		if (pMesh->HasVertexColors(6)) descriptor.m_Color6 = Flint::CoordinateType::R8G8B8;
		if (pMesh->HasVertexColors(7)) descriptor.m_Color7 = Flint::CoordinateType::R8G8B8;

		// Resolve texture coordinates.
		if (pMesh->HasTextureCoords(0)) descriptor.m_Texture0 = Flint::CoordinateType::U8V8;
		if (pMesh->HasTextureCoords(1)) descriptor.m_Texture1 = Flint::CoordinateType::U8V8;
		if (pMesh->HasTextureCoords(2)) descriptor.m_Texture2 = Flint::CoordinateType::U8V8;
		if (pMesh->HasTextureCoords(3)) descriptor.m_Texture3 = Flint::CoordinateType::U8V8;
		if (pMesh->HasTextureCoords(4)) descriptor.m_Texture4 = Flint::CoordinateType::U8V8;
		if (pMesh->HasTextureCoords(5)) descriptor.m_Texture5 = Flint::CoordinateType::U8V8;
		if (pMesh->HasTextureCoords(6)) descriptor.m_Texture6 = Flint::CoordinateType::U8V8;
		if (pMesh->HasTextureCoords(7)) descriptor.m_Texture7 = Flint::CoordinateType::U8V8;

		// Get the index count.
		for (uint32_t i = 0; i < pMesh->mNumFaces; i++)
			indices += pMesh->mFaces[i].mNumIndices;

		return std::make_tuple(descriptor, pMesh->mNumVertices, indices);
	}

	/**
	 * Copy data from one pointer to another and increment the destination pointer.
	 *
	 * @param pSource The source data pointer.
	 * @param pDestination The destination pointer.
	 * @param size The number of bytes to copy.
	 */
	void CopyData(const std::byte* pSource, std::byte*& pDestination, uint8_t size)
	{
		std::copy_n(std::execution::unseq, pSource, size, pDestination);
		pDestination += size;
	}

	/**
	 * Load the mesh data.
	 *
	 * @param pScene The scene pointer.
	 * @param pMesh The mesh pointer.
	 * @param pVertex The vertex buffer pointer.
	 * @param pIndex The index buffer pointer.
	 * @param descriptor The vertex descriptor specifying what to load.
	 */
	void LoadMesh(const aiScene* pScene, const aiMesh* pMesh, std::byte* pVertex, std::byte* pIndex, Flint::VertexDescriptor descriptor)
	{
		// Load the vertex data.
		for (uint32_t i = 0; i < pMesh->mNumVertices; ++i)
		{
			// Copy position.
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mVertices[i].x), pVertex, static_cast<uint8_t>(descriptor.m_Position));

			// Copy normal.
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mNormals[i].x), pVertex, static_cast<uint8_t>(descriptor.m_Normal));

			// COpy tangent.
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mTangents[i].x), pVertex, static_cast<uint8_t>(descriptor.m_Tangent));

			// COpy bi-tangent.
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mBitangents[i].x), pVertex, static_cast<uint8_t>(descriptor.m_BiTangent));

			// Copy colors.
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mColors[0][i].r), pVertex, static_cast<uint8_t>(descriptor.m_Color0));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mColors[1][i].r), pVertex, static_cast<uint8_t>(descriptor.m_Color1));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mColors[2][i].r), pVertex, static_cast<uint8_t>(descriptor.m_Color2));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mColors[3][i].r), pVertex, static_cast<uint8_t>(descriptor.m_Color3));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mColors[4][i].r), pVertex, static_cast<uint8_t>(descriptor.m_Color4));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mColors[5][i].r), pVertex, static_cast<uint8_t>(descriptor.m_Color5));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mColors[6][i].r), pVertex, static_cast<uint8_t>(descriptor.m_Color6));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mColors[7][i].r), pVertex, static_cast<uint8_t>(descriptor.m_Color7));

			// Copy texture coordinates.
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mTextureCoords[0][i].x), pVertex, static_cast<uint8_t>(descriptor.m_Texture0));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mTextureCoords[1][i].x), pVertex, static_cast<uint8_t>(descriptor.m_Texture1));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mTextureCoords[2][i].x), pVertex, static_cast<uint8_t>(descriptor.m_Texture2));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mTextureCoords[3][i].x), pVertex, static_cast<uint8_t>(descriptor.m_Texture3));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mTextureCoords[4][i].x), pVertex, static_cast<uint8_t>(descriptor.m_Texture4));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mTextureCoords[5][i].x), pVertex, static_cast<uint8_t>(descriptor.m_Texture5));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mTextureCoords[6][i].x), pVertex, static_cast<uint8_t>(descriptor.m_Texture6));
			CopyData(reinterpret_cast<std::byte*>(&pMesh->mTextureCoords[7][i].x), pVertex, static_cast<uint8_t>(descriptor.m_Texture7));
		}

		// Load the index data.
		aiFace face = {};
		uint32_t* pIndexData = reinterpret_cast<uint32_t*>(pIndex);
		for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
		{
			face = pMesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; ++j, ++pIndexData)
				*pIndexData = face.mIndices[j];
		}

		// Load materials.
		const auto pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];
		for (uint32_t i = 0; i < pMaterial->mNumProperties; i++)
		{
			const auto pProperty = pMaterial->mProperties[i];
		}
	}

	/**
	 * Walk the node tree and load in assets.
	 *
	 * @param pScene The scene pointer.
	 * @param pNode The node pointer.
	 * @param geometry The geometry to load the data to.
	 */
	void WalkNodeTree(const aiScene* pScene, const aiNode* pNode, Flint::Geometry& geometry)
	{
		// Load the meshes.
		for (uint32_t i = 0; i < pNode->mNumMeshes; i++)
		{
			const auto pMesh = pScene->mMeshes[pNode->mMeshes[i]];
			const auto [descriptor, vertices, indices] = GetBasicMeshData(pMesh);
			auto& mesh = geometry.createMesh(descriptor, vertices, indices);

			LoadMesh(pScene, pMesh, mesh.mapVertexMemory(), mesh.mapIndexMemory(), descriptor);
			mesh.unmapVertexMemory();
			mesh.unmapIndexMemory();
		}

		// Walk the children and load all the meshes.
		for (uint32_t child = 0; child < pNode->mNumChildren; child++)
			WalkNodeTree(pScene, pNode->mChildren[child], geometry);
	}
}

namespace Flint
{
	void StaticEntity::loadModel(std::filesystem::path&& path)
	{
		OPTICK_EVENT();

		// Setup the importer and the load the scene.
		Assimp::Importer importer = {};
		const aiScene* pScene = importer.ReadFile(path.string(),
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices |
			aiProcess_Triangulate |
			//aiProcess_OptimizeMeshes |
			//aiProcess_OptimizeGraph |
			aiProcess_SortByPType |
			aiProcess_GenUVCoords |
			aiProcess_FlipUVs);

		// If the scene was not loaded properly, we can throw an exception.
		if (!pScene)
			throw AssetError("Failed to load the specified model!");

		// Load the mesh data.
		uint64_t vertexSize = 0, indexSize = 0;
		for (uint32_t i = 0; i < pScene->mNumMeshes; i++)
		{
			const auto [descriptor, vertices, indices] = GetBasicMeshData(pScene->mMeshes[i]);

			vertexSize += descriptor.getStride() * vertices;
			indexSize += sizeof(uint32_t) * indices;
		}

		// Create the geometry.
		m_Geometry = getEngine().getDefaultGeometryStore().createGeometry(vertexSize, indexSize);

		// Walk through the meshes.
		WalkNodeTree(pScene, pScene->mRootNode, m_Geometry);
	}
}