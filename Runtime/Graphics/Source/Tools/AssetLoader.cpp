// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Tools/AssetLoader.hpp"

#include "GraphicsCore/Device.hpp"
#include "GraphicsCore/Material.hpp"

#include "Graphics/Tools/ImageLoader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <future>
#include <optick.h>

#ifdef FLINT_PLATFORM_WINDOWS
constexpr const char* LineEnding = "\\";

#else
constexpr const char* LineEnding = "/";

#endif // FLINT_PLATFORM_WINDOWS

namespace Flint
{
	namespace Helpers
	{
		MaterialProperties::TextureType GetTextureType(aiTextureType type)
		{
			switch (type)
			{
			case aiTextureType_DIFFUSE:
				return MaterialProperties::TextureType::Diffuse;

			case aiTextureType_SPECULAR:
				return MaterialProperties::TextureType::Specular;

			case aiTextureType_AMBIENT:
				return MaterialProperties::TextureType::Ambient;

			case aiTextureType_EMISSIVE:
				return MaterialProperties::TextureType::Emissive;

			case aiTextureType_HEIGHT:
				return MaterialProperties::TextureType::Height;

			case aiTextureType_NORMALS:
				return MaterialProperties::TextureType::Normals;

			case aiTextureType_SHININESS:
				return MaterialProperties::TextureType::Shininess;

			case aiTextureType_OPACITY:
				return MaterialProperties::TextureType::Opacity;

			case aiTextureType_DISPLACEMENT:
				return MaterialProperties::TextureType::Displacement;

			case aiTextureType_LIGHTMAP:
				return MaterialProperties::TextureType::Lightmap;

			case aiTextureType_REFLECTION:
				return MaterialProperties::TextureType::Reflection;

			case aiTextureType_BASE_COLOR:
				return MaterialProperties::TextureType::BaseColor;

			case aiTextureType_NORMAL_CAMERA:
				return MaterialProperties::TextureType::NormalCamera;

			case aiTextureType_EMISSION_COLOR:
				return MaterialProperties::TextureType::EmissionColor;

			case aiTextureType_METALNESS:
				return MaterialProperties::TextureType::Metalness;

			case aiTextureType_DIFFUSE_ROUGHNESS:
				return MaterialProperties::TextureType::DiffuseRoughness;

			case aiTextureType_AMBIENT_OCCLUSION:
				return MaterialProperties::TextureType::AmbientOcclusion;

			case aiTextureType_TRANSMISSION:
				return MaterialProperties::TextureType::Transmission;

			default:
				return MaterialProperties::TextureType::Undefined;
			}

			return MaterialProperties::TextureType::Undefined;
		}

		void GetTextures(Material& material, const std::string basePath, const aiMaterial* pMaterial, const aiTextureType type)
		{
			OPTICK_EVENT();

			const uint32_t count = pMaterial->GetTextureCount(type);
			for (uint32_t i = 0; i < count; i++)
			{
				aiString path = {};

				if (pMaterial->GetTexture(type, i, &path) == aiReturn::aiReturn_SUCCESS)
				{
					material.AddProperty(std::make_shared<MaterialProperties::TexturePath>(basePath + path.C_Str(), GetTextureType(type)));
				}
			}
		}

		void GetColors(const char* pKey, const uint32_t type, const uint32_t index, Material& material, const aiMaterial* pMaterial)
		{
			OPTICK_EVENT();

			aiColor4D color(0.0f, 0.0f, 0.0f, 0.0f);
			if (pMaterial->Get(pKey, type, index, color) == aiReturn::aiReturn_SUCCESS)
			{
				float value[4] = { color.r, color.g, color.b, color.a };
				material.AddProperty(std::make_shared<MaterialProperties::Float4>(value));
			}
		}

		void LoadWireFrame(
			const aiMesh* pMesh,
			const aiMaterial* pMaterial,
			const std::string_view& basePath,
			const VertexDescriptor vertexDescriptor,
			float* pBufferMemory,
			uint64_t vertexOffset,
			std::vector<uint32_t>* pIndexes,
			WireFrame* pWireFrame)
		{
			OPTICK_THREAD("Wire Frame Loader");
			OPTICK_EVENT();

			//std::vector<aiMaterialProperty*> pProperties(pMaterial->m_Properties, pMaterial->m_Properties + pMaterial->m_NumProperties);
			Material material = {};

			// Load colors.
			{
				Helpers::GetColors(AI_MATKEY_COLOR_DIFFUSE, material, pMaterial);
				Helpers::GetColors(AI_MATKEY_COLOR_AMBIENT, material, pMaterial);
				Helpers::GetColors(AI_MATKEY_COLOR_SPECULAR, material, pMaterial);
				Helpers::GetColors(AI_MATKEY_COLOR_EMISSIVE, material, pMaterial);
				Helpers::GetColors(AI_MATKEY_COLOR_TRANSPARENT, material, pMaterial);
				Helpers::GetColors(AI_MATKEY_COLOR_REFLECTIVE, material, pMaterial);
			}

			// Load the textures.
			{
				for (uint32_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
					Helpers::GetTextures(material, basePath.data(), pMaterial, static_cast<aiTextureType>(i));
			}

			// Load vertexes.
			for (uint32_t j = 0; j < pMesh->m_NumVertices; j++)
			{
				for (const auto attribute : vertexDescriptor.m_Attributes)
				{
					const uint64_t copyAmount = attribute.m_AttributeSize / sizeof(float);

					switch (attribute.m_Type)
					{
					case InputAttributeType::Position:
						if (pMesh->HasPositions())
							std::copy(&pMesh->m_Vertices[j].x, (&pMesh->m_Vertices[j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::Normal:
						if (pMesh->HasNormals())
							std::copy(&pMesh->m_Normals[j].x, (&pMesh->m_Normals[j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::ColorZero:
						if (pMesh->HasVertexColors(0))
							std::copy(&pMesh->m_Colors[0][j].r, (&pMesh->m_Colors[0][j].r) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::ColorOne:
						if (pMesh->HasVertexColors(1))
							std::copy(&pMesh->m_Colors[1][j].r, (&pMesh->m_Colors[1][j].r) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::ColorTwo:
						if (pMesh->HasVertexColors(2))
							std::copy(&pMesh->m_Colors[2][j].r, (&pMesh->m_Colors[1][j].r) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::ColorThree:
						if (pMesh->HasVertexColors(3))
							std::copy(&pMesh->m_Colors[3][j].r, (&pMesh->m_Colors[2][j].r) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesZero:
						if (pMesh->HasTextureCoords(0))
							std::copy(&pMesh->m_TextureCoords[0][j].x, (&pMesh->m_TextureCoords[0][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesOne:
						if (pMesh->HasTextureCoords(1))
							std::copy(&pMesh->m_TextureCoords[1][j].x, (&pMesh->m_TextureCoords[1][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesTwo:
						if (pMesh->HasTextureCoords(2))
							std::copy(&pMesh->m_TextureCoords[2][j].x, (&pMesh->m_TextureCoords[2][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesThree:
						if (pMesh->HasTextureCoords(3))
							std::copy(&pMesh->m_TextureCoords[3][j].x, (&pMesh->m_TextureCoords[3][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesFour:
						if (pMesh->HasTextureCoords(4))
							std::copy(&pMesh->m_TextureCoords[4][j].x, (&pMesh->m_TextureCoords[4][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesFive:
						if (pMesh->HasTextureCoords(5))
							std::copy(&pMesh->m_TextureCoords[5][j].x, (&pMesh->m_TextureCoords[5][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesSix:
						if (pMesh->HasTextureCoords(6))
							std::copy(&pMesh->m_TextureCoords[6][j].x, (&pMesh->m_TextureCoords[6][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesSeven:
						if (pMesh->HasTextureCoords(7))
							std::copy(&pMesh->m_TextureCoords[7][j].x, (&pMesh->m_TextureCoords[7][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::UVCoordinates:
						if (pMesh->HasTextureCoords(0))
							std::copy(&pMesh->m_TextureCoords[0][j].x, (&pMesh->m_TextureCoords[0][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::Tangent:
						if (pMesh->HasTangentsAndBitangents())
							std::copy(&pMesh->m_Tangents[j].x, (&pMesh->m_Tangents[j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::Bitangent:
						if (pMesh->HasTangentsAndBitangents())
							std::copy(&pMesh->m_Bitangents[j].x, (&pMesh->m_Bitangents[j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::BoneID:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->m_Vertices[j].x, (&pMesh->m_Vertices[j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::BoneWeight:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->m_Vertices[j].x, (&pMesh->m_Vertices[j].x) + copyAmount, pBufferMemory);
						break;

					default:
						throw std::runtime_error("Invalid or Undefined vertex attribute type!");
					}

					pBufferMemory = pBufferMemory + copyAmount;
				}
			}

			aiFace face = {};
			for (uint32_t j = 0; j < pMesh->m_NumFaces; j++)
			{
				face = pMesh->m_Faces[j];
				for (uint32_t k = 0; k < face.m_NumIndices; k++)
					pIndexes->push_back(face.m_Indices[k]);
			}

			const uint64_t indexCount = pIndexes->size();
			(*pWireFrame) = WireFrame(pMesh->m_Name.C_Str(), vertexOffset, pMesh->m_NumVertices, 0, indexCount, material);
		}
	}

	const uint64_t VertexDescriptor::Size() const
	{
		uint64_t size = 0;
		for (const auto attribute : m_Attributes)
			size += attribute.m_AttributeSize;

		return size;
	}

	AssetLoader::AssetLoader(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::filesystem::path& assetPath, const VertexDescriptor& vertexDescriptor, const bool useMultiThreading)
		: pGeometryStore(pGeometryStore), m_Descriptor(vertexDescriptor)
	{
		// Validate the geometry store and vertex descriptor.
		if (pGeometryStore->GetVertexSize() != vertexDescriptor.Size())
			throw std::invalid_argument("The vertex sizes mismatch! Make sure that the geometry store's vertex size and the vertex descriptor's sizes match.");

		Assimp::Importer importer = {};
		const aiScene* pScene = importer.ReadFile(assetPath.string(),
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices |
			aiProcess_Triangulate |
			//aiProcess_OptimizeMeshes |
			//aiProcess_OptimizeGraph |
			aiProcess_SortByPType |
			aiProcess_GenUVCoords |
			aiProcess_FlipUVs);

		// Check if the scene could be loaded.
		if (!pScene)
			throw std::invalid_argument("Provided asset could not be loaded!");

		const uint64_t vertexSize = vertexDescriptor.Size();
		uint64_t vertexCount = 0;

		m_WireFrames.resize(pScene->m_NumMeshes);
		for (uint32_t i = 0; i < pScene->m_NumMeshes; i++)
			vertexCount += pScene->m_Meshes[i]->m_NumVertices;

		const auto pDevice = pGeometryStore->GetDevice();
		const auto pVertexStagingBuffer = pDevice->CreateBuffer(Flint::BufferType::Staging, vertexSize * vertexCount);
		const auto pBufferMemory = static_cast<float*>(pVertexStagingBuffer->MapMemory(vertexSize * vertexCount));

		// Load the mesh data.
		std::vector<std::vector<uint32_t>> indexData{ pScene->m_NumMeshes };

		{
			std::vector<std::future<void>> futures;
			futures.reserve(pScene->m_NumMeshes);

			const uint64_t vertexFloatCount = vertexDescriptor.Size() / sizeof(float);
			const auto basePath = assetPath.parent_path().string() + LineEnding;

			uint64_t vertexOffset = 0;
			for (uint32_t i = 0; i < pScene->m_NumMeshes; i++)
			{
				const auto pMesh = pScene->m_Meshes[i];
				const auto pMaterial = pScene->m_Materials[pMesh->m_MaterialIndex];

				if (useMultiThreading)
					futures.emplace_back(std::async(std::launch::async, Helpers::LoadWireFrame, pMesh, pMaterial, basePath, vertexDescriptor, pBufferMemory + (vertexOffset * vertexFloatCount), vertexOffset, &indexData[i], &m_WireFrames[i]));
				else
					Helpers::LoadWireFrame(pMesh, pMaterial, basePath, vertexDescriptor, pBufferMemory + (vertexOffset * vertexFloatCount), vertexOffset, &indexData[i], &m_WireFrames[i]);

				vertexOffset += pMesh->m_NumVertices;
			}
		}

		pVertexStagingBuffer->UnmapMemory();

		uint64_t indexCount = 0;
		for (auto& indexes : indexData)
			indexCount += indexes.size();

		const auto pIndexStagingBuffer = pDevice->CreateBuffer(Flint::BufferType::Staging, indexCount * sizeof(uint32_t));
		const auto pIndexMemory = static_cast<uint32_t*>(pIndexStagingBuffer->MapMemory(indexCount * sizeof(uint32_t)));

		uint64_t offset = 0;
		for (auto& indexes : indexData)
		{
			std::copy(indexes.begin(), indexes.end(), pIndexMemory + offset);
			offset += indexes.size();
		}

		indexData.clear();
		pIndexStagingBuffer->UnmapMemory();

		const auto [vertexGeometryOffset, indexGeometryOffset] = pGeometryStore->AddGeometry(pVertexStagingBuffer.get(), pIndexStagingBuffer.get());
		uint64_t indexOffset = 0;
		for (auto& wireFrame : m_WireFrames)
		{
			wireFrame.SetVertexOffset(wireFrame.GetVertexOffset() + vertexGeometryOffset);
			wireFrame.SetIndexOffset(indexOffset + indexGeometryOffset);
			indexOffset += wireFrame.GetIndexCount();
		}

		pVertexStagingBuffer->Terminate();
		pIndexStagingBuffer->Terminate();
	}

	namespace Defaults
	{
		VertexDescriptor CreateDefaultVertexDescriptor()
		{
			Flint::VertexDescriptor vDescriptor = {};
			vDescriptor.m_Attributes.emplace_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::InputAttributeType::Position));
			vDescriptor.m_Attributes.emplace_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::InputAttributeType::ColorZero));
			vDescriptor.m_Attributes.emplace_back(Flint::VertexAttribute(sizeof(float) * 2, Flint::InputAttributeType::TextureCoordinatesZero));

			return vDescriptor;
		}

		VertexDescriptor CreateSkyBoxVertexDescriptor()
		{
			Flint::VertexDescriptor vDescriptor = {};
			vDescriptor.m_Attributes.emplace_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::InputAttributeType::Position));
			vDescriptor.m_Attributes.emplace_back(Flint::VertexAttribute(sizeof(float) * 2, Flint::InputAttributeType::TextureCoordinatesZero));

			return vDescriptor;
		}
	}
}
