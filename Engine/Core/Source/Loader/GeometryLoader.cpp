// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Loader/GeometryLoader.hpp"
#include "Core/GeometryStore.hpp"
#include "Core/Errors/AssetError.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <optick.h>

namespace /* anonymous */
{
	/**
	 * Get the basic mesh information.
	 *
	 * @param pMesh The mesh pointer.
	 * @param loadData The data that we need to load.
	 * @return The descriptor, vertex count and index count.
	 */
	std::tuple<Flint::VertexDescriptor, uint64_t, uint64_t> GetBasicMeshData(const aiMesh* pMesh, Flint::VertexData loadData)
	{
		Flint::VertexDescriptor descriptor = {};
		uint64_t vertices = 0, indices = 0;

		// Get the vertex information.
		if (pMesh->HasPositions() && (loadData & Flint::VertexData::Position) == Flint::VertexData::Position)		descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Position)] = Flint::DataType::Vec3_32;
		if (pMesh->HasNormals() && (loadData & Flint::VertexData::Normal) == Flint::VertexData::Normal)				descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Normal)] = Flint::DataType::Vec3_32;

		if (pMesh->HasTangentsAndBitangents())
		{
			if ((loadData & Flint::VertexData::Tangent) == Flint::VertexData::Tangent)								descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Tangent)] = Flint::DataType::Vec3_32;
			if ((loadData & Flint::VertexData::BiTangent) == Flint::VertexData::BiTangent)							descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::BiTangent)] = Flint::DataType::Vec3_32;
		}

		// Resolve vertex colors.
		if (pMesh->HasVertexColors(0) && (loadData & Flint::VertexData::Color0) == Flint::VertexData::Color0)		descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color0)] = Flint::DataType::Vec4_32;
		if (pMesh->HasVertexColors(1) && (loadData & Flint::VertexData::Color1) == Flint::VertexData::Color1)		descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color1)] = Flint::DataType::Vec4_32;
		if (pMesh->HasVertexColors(2) && (loadData & Flint::VertexData::Color2) == Flint::VertexData::Color2)		descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color2)] = Flint::DataType::Vec4_32;
		if (pMesh->HasVertexColors(3) && (loadData & Flint::VertexData::Color3) == Flint::VertexData::Color3)		descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color3)] = Flint::DataType::Vec4_32;
		if (pMesh->HasVertexColors(4) && (loadData & Flint::VertexData::Color4) == Flint::VertexData::Color4)		descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color4)] = Flint::DataType::Vec4_32;
		if (pMesh->HasVertexColors(5) && (loadData & Flint::VertexData::Color5) == Flint::VertexData::Color5)		descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color5)] = Flint::DataType::Vec4_32;
		if (pMesh->HasVertexColors(6) && (loadData & Flint::VertexData::Color6) == Flint::VertexData::Color6)		descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color6)] = Flint::DataType::Vec4_32;
		if (pMesh->HasVertexColors(7) && (loadData & Flint::VertexData::Color7) == Flint::VertexData::Color7)		descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color7)] = Flint::DataType::Vec4_32;

		// Resolve texture coordinates.
		if (pMesh->HasTextureCoords(0) && (loadData & Flint::VertexData::Texture0) == Flint::VertexData::Texture0) descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture0)] = Flint::DataType::Vec2_32;
		if (pMesh->HasTextureCoords(1) && (loadData & Flint::VertexData::Texture1) == Flint::VertexData::Texture1) descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture1)] = Flint::DataType::Vec2_32;
		if (pMesh->HasTextureCoords(2) && (loadData & Flint::VertexData::Texture2) == Flint::VertexData::Texture2) descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture2)] = Flint::DataType::Vec2_32;
		if (pMesh->HasTextureCoords(3) && (loadData & Flint::VertexData::Texture3) == Flint::VertexData::Texture3) descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture3)] = Flint::DataType::Vec2_32;
		if (pMesh->HasTextureCoords(4) && (loadData & Flint::VertexData::Texture4) == Flint::VertexData::Texture4) descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture4)] = Flint::DataType::Vec2_32;
		if (pMesh->HasTextureCoords(5) && (loadData & Flint::VertexData::Texture5) == Flint::VertexData::Texture5) descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture5)] = Flint::DataType::Vec2_32;
		if (pMesh->HasTextureCoords(6) && (loadData & Flint::VertexData::Texture6) == Flint::VertexData::Texture6) descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture6)] = Flint::DataType::Vec2_32;
		if (pMesh->HasTextureCoords(7) && (loadData & Flint::VertexData::Texture7) == Flint::VertexData::Texture7) descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture7)] = Flint::DataType::Vec2_32;

		// Get the index count.
		for (uint32_t i = 0; i < pMesh->mNumFaces; i++)
			indices += pMesh->mFaces[i].mNumIndices;

		return std::make_tuple(descriptor, pMesh->mNumVertices, indices);
	}

	/**
	 * Copy data from a 3D vector to the destination.
	 *
	 * @param source The source data.
	 * @param pDestination The destination data pointer.
	 * @param type The amount of data to copy.
	 */
	void CopyData(const aiVector3D& source, float*& pDestination, Flint::DataType type) noexcept
	{
		// Fill default data if required.
		if (type != Flint::DataType::None)
		{
			*(pDestination++) = source[0];
			*(pDestination++) = source[1];
			*(pDestination++) = source[2];
		}
	}

	/**
	 * Copy data from a 4D vector to the destination.
	 *
	 * @param source The source data.
	 * @param pDestination The destination data pointer.
	 * @param type The amount of data to copy.
	 */
	void CopyData(const aiColor4D& source, float*& pDestination, Flint::DataType type) noexcept
	{
		// Fill default data if required.
		if (type != Flint::DataType::None)
		{
			*(pDestination++) = source[0];
			*(pDestination++) = source[1];
			*(pDestination++) = source[2];
			*(pDestination++) = source[3];
		}
	}

	/**
	 * Load color from material.
	 *
	 * @param mesh The mesh to load the material to.
	 * @param pMaterial The material pointer.
	 * @param pKey The key string.
	 * @param type The color type.
	 * @param index The color index.
	 * @param colorType The flint color type.
	 */
	void LoadColorMaterial(Flint::Mesh& mesh, const aiMaterial* pMaterial, const char* pKey, uint32_t type, uint32_t index, Flint::ColorType colorType)
	{
		aiColor4D color;
		if (pMaterial->Get(pKey, type, index, color) == aiReturn::aiReturn_SUCCESS)
			mesh.addMaterial(color[0], color[1], color[2], color[3], colorType);
	}

	/**
	 * Load texture from material.
	 *
	 * @param mesh The mesh to load the material to.
	 * @param pMaterial The material pointer.
	 * @param type The texture type.
	 * @param index The texture index.
	 * @param textureType The flint texture type.
	 * @param basePath The base path of the file.
	 */
	void LoadTextureMaterial(Flint::Mesh& mesh, const aiMaterial* pMaterial, aiTextureType type, uint32_t index, Flint::TextureType textureType, const std::filesystem::path& basePath)
	{
		aiString filePath;
		if (pMaterial->GetTexture(type, index, &filePath) == aiReturn::aiReturn_SUCCESS)
			mesh.addMaterial(basePath / filePath.C_Str(), textureType);
	}

	/**
	 * Load the mesh data.
	 *
	 * @param pScene The scene pointer.
	 * @param pMesh The mesh pointer.
	 * @param geometry The geometry to which this mesh is loaded to.
	 * @param basePath The base path of the file.
	 * @param loadData The data to load.
	 */
	void LoadMesh(const aiScene* pScene, const aiMesh* pMesh, Flint::Geometry& geometry, const std::filesystem::path& basePath, Flint::VertexData loadData)
	{
		// First, get the basic information required to create the mesh.
		const auto [descriptor, vertices, indices] = GetBasicMeshData(pMesh, loadData);

		// Now we can create the mesh and load the data.
		auto& mesh = geometry.createMesh(pMesh->mName.C_Str(), descriptor, vertices, indices);

		auto pVertex = reinterpret_cast<float*>(mesh.mapVertexMemory(geometry));
		auto pIndex = mesh.mapIndexMemory(geometry);

		// Load the vertex data.
		for (uint32_t i = 0; i < pMesh->mNumVertices; ++i)
		{
			// Copy position.
			CopyData(pMesh->mVertices[i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Position)]);

			// Copy normal.
			CopyData(pMesh->mNormals[i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Normal)]);

			// COpy tangent.
			CopyData(pMesh->mTangents[i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Tangent)]);

			// COpy bi-tangent.
			CopyData(pMesh->mBitangents[i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::BiTangent)]);

			// Copy colors.
			CopyData(pMesh->mColors[0][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color0)]);
			CopyData(pMesh->mColors[1][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color1)]);
			CopyData(pMesh->mColors[2][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color2)]);
			CopyData(pMesh->mColors[3][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color3)]);
			CopyData(pMesh->mColors[4][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color4)]);
			CopyData(pMesh->mColors[5][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color5)]);
			CopyData(pMesh->mColors[6][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color6)]);
			CopyData(pMesh->mColors[7][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Color7)]);

			// Copy texture coordinates.
			CopyData(pMesh->mTextureCoords[0][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture0)]);
			CopyData(pMesh->mTextureCoords[1][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture1)]);
			CopyData(pMesh->mTextureCoords[2][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture2)]);
			CopyData(pMesh->mTextureCoords[3][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture3)]);
			CopyData(pMesh->mTextureCoords[4][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture4)]);
			CopyData(pMesh->mTextureCoords[5][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture5)]);
			CopyData(pMesh->mTextureCoords[6][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture6)]);
			CopyData(pMesh->mTextureCoords[7][i], pVertex, descriptor[static_cast<std::underlying_type_t<Flint::VertexAttribute>>(Flint::VertexAttribute::Texture7)]);
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

		// Unmap the memory.
		mesh.unmapVertexMemory(geometry);
		mesh.unmapIndexMemory(geometry);

		// Load materials.
		const auto pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

		// First load the color material.
		LoadColorMaterial(mesh, pMaterial, AI_MATKEY_COLOR_DIFFUSE, Flint::ColorType::Diffuse);
		LoadColorMaterial(mesh, pMaterial, AI_MATKEY_COLOR_AMBIENT, Flint::ColorType::Ambient);
		LoadColorMaterial(mesh, pMaterial, AI_MATKEY_COLOR_SPECULAR, Flint::ColorType::Specular);
		LoadColorMaterial(mesh, pMaterial, AI_MATKEY_COLOR_EMISSIVE, Flint::ColorType::Emissive);
		LoadColorMaterial(mesh, pMaterial, AI_MATKEY_COLOR_TRANSPARENT, Flint::ColorType::Transparent);
		LoadColorMaterial(mesh, pMaterial, AI_MATKEY_COLOR_REFLECTIVE, Flint::ColorType::Reflective);
		LoadColorMaterial(mesh, pMaterial, AI_MATKEY_BASE_COLOR, Flint::ColorType::Base);

		// Then load the textures.
		LoadTextureMaterial(mesh, pMaterial, AI_MATKEY_BASE_COLOR_TEXTURE, Flint::TextureType::BaseColor, basePath);
		LoadTextureMaterial(mesh, pMaterial, AI_MATKEY_ROUGHNESS_TEXTURE, Flint::TextureType::DiffuseRoughness, basePath);
		LoadTextureMaterial(mesh, pMaterial, AI_MATKEY_SHEEN_COLOR_TEXTURE, Flint::TextureType::SheenColor, basePath);
		LoadTextureMaterial(mesh, pMaterial, AI_MATKEY_SHEEN_ROUGHNESS_TEXTURE, Flint::TextureType::SheenRoughness, basePath);
		LoadTextureMaterial(mesh, pMaterial, AI_MATKEY_CLEARCOAT_TEXTURE, Flint::TextureType::ClearCoat, basePath);
		LoadTextureMaterial(mesh, pMaterial, AI_MATKEY_CLEARCOAT_ROUGHNESS_TEXTURE, Flint::TextureType::ClearCoatRoughness, basePath);
		LoadTextureMaterial(mesh, pMaterial, AI_MATKEY_CLEARCOAT_NORMAL_TEXTURE, Flint::TextureType::ClearCoatNormal, basePath);
		LoadTextureMaterial(mesh, pMaterial, AI_MATKEY_TRANSMISSION_TEXTURE, Flint::TextureType::Transmission, basePath);
	}

	/**
	 * Walk the node tree and load in assets.
	 *
	 * @param pScene The scene pointer.
	 * @param pNode The node pointer.
	 * @param geometry The geometry to load the data to.
	 * @param basePath The base path of the file.
	 * @param loadData The data that we need to load.
	 */
	void WalkNodeTree(const aiScene* pScene, const aiNode* pNode, Flint::Geometry& geometry, const std::filesystem::path& basePath, Flint::VertexData loadData)
	{
		// Load the meshes.
		for (uint32_t i = 0; i < pNode->mNumMeshes; i++)
			LoadMesh(pScene, pScene->mMeshes[pNode->mMeshes[i]], geometry, basePath, loadData);

		// Walk the children and load all the meshes.
		for (uint32_t child = 0; child < pNode->mNumChildren; child++)
			WalkNodeTree(pScene, pNode->mChildren[child], geometry, basePath, loadData);
	}
}

namespace Flint
{
	Flint::Geometry LoadGeometry(GeometryStore& store, std::filesystem::path&& path, VertexData vertexData /*= VertexData::All*/)
	{
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
			const auto [descriptor, vertices, indices] = GetBasicMeshData(pScene->mMeshes[i], vertexData);

			vertexSize += GetStride(descriptor) * vertices;
			indexSize += sizeof(uint32_t) * indices;
		}

		// Create the geometry.
		auto geometry = store.createGeometry(vertexSize, indexSize);

		// Walk through the meshes.
		WalkNodeTree(pScene, pScene->mRootNode, geometry, path.parent_path(), vertexData);

		return geometry;
	}
}