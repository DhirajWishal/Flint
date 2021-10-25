// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/AssetLoader.hpp"

#include "GraphicsCore/Device.hpp"
#include "GraphicsCore/Material.hpp"

#include "Graphics/ImageLoader.hpp"

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

			const UI32 count = pMaterial->GetTextureCount(type);
			for (UI32 i = 0; i < count; i++)
			{
				aiString path = {};

				if (pMaterial->GetTexture(type, i, &path) == aiReturn::aiReturn_SUCCESS)
				{
					material.AddProperty(std::make_shared<MaterialProperties::TexturePath>(basePath + path.C_Str(), GetTextureType(type)));
				}
			}
		}

		void GetColors(const char* pKey, const UI32 type, const UI32 index, Material& material, const aiMaterial* pMaterial)
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
			UI64 vertexOffset,
			std::vector<UI32>* pIndexes,
			WireFrame* pWireFrame)
		{
			OPTICK_THREAD("Wire Frame Loader");
			OPTICK_EVENT();

			//std::vector<aiMaterialProperty*> pProperties(pMaterial->mProperties, pMaterial->mProperties + pMaterial->mNumProperties);
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
				for (UI32 i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
					Helpers::GetTextures(material, basePath.data(), pMaterial, static_cast<aiTextureType>(i));
			}

			// Load vertexes.
			for (UI32 j = 0; j < pMesh->mNumVertices; j++)
			{
				for (const auto attribute : vertexDescriptor.mAttributes)
				{
					const UI64 copyAmount = attribute.mAttributeSize / sizeof(float);

					switch (attribute.mType)
					{
					case InputAttributeType::Position:
						if (pMesh->HasPositions())
							std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::Normal:
						if (pMesh->HasNormals())
							std::copy(&pMesh->mNormals[j].x, (&pMesh->mNormals[j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::ColorZero:
						if (pMesh->HasVertexColors(0))
							std::copy(&pMesh->mColors[0][j].r, (&pMesh->mColors[0][j].r) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::ColorOne:
						if (pMesh->HasVertexColors(1))
							std::copy(&pMesh->mColors[1][j].r, (&pMesh->mColors[1][j].r) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::ColorTwo:
						if (pMesh->HasVertexColors(2))
							std::copy(&pMesh->mColors[2][j].r, (&pMesh->mColors[1][j].r) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::ColorThree:
						if (pMesh->HasVertexColors(3))
							std::copy(&pMesh->mColors[3][j].r, (&pMesh->mColors[2][j].r) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesZero:
						if (pMesh->HasTextureCoords(0))
							std::copy(&pMesh->mTextureCoords[0][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesOne:
						if (pMesh->HasTextureCoords(1))
							std::copy(&pMesh->mTextureCoords[1][j].x, (&pMesh->mTextureCoords[1][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesTwo:
						if (pMesh->HasTextureCoords(2))
							std::copy(&pMesh->mTextureCoords[2][j].x, (&pMesh->mTextureCoords[2][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesThree:
						if (pMesh->HasTextureCoords(3))
							std::copy(&pMesh->mTextureCoords[3][j].x, (&pMesh->mTextureCoords[3][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesFour:
						if (pMesh->HasTextureCoords(4))
							std::copy(&pMesh->mTextureCoords[4][j].x, (&pMesh->mTextureCoords[4][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesFive:
						if (pMesh->HasTextureCoords(5))
							std::copy(&pMesh->mTextureCoords[5][j].x, (&pMesh->mTextureCoords[5][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesSix:
						if (pMesh->HasTextureCoords(6))
							std::copy(&pMesh->mTextureCoords[6][j].x, (&pMesh->mTextureCoords[6][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::TextureCoordinatesSeven:
						if (pMesh->HasTextureCoords(7))
							std::copy(&pMesh->mTextureCoords[7][j].x, (&pMesh->mTextureCoords[7][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::UVCoordinates:
						if (pMesh->HasTextureCoords(0))
							std::copy(&pMesh->mTextureCoords[0][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::Tangent:
						if (pMesh->HasTangentsAndBitangents())
							std::copy(&pMesh->mTangents[j].x, (&pMesh->mTangents[j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::Bitangent:
						if (pMesh->HasTangentsAndBitangents())
							std::copy(&pMesh->mBitangents[j].x, (&pMesh->mBitangents[j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::BoneID:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pBufferMemory);
						break;

					case InputAttributeType::BoneWeight:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pBufferMemory);
						break;

					default:
						throw std::runtime_error("Invalid or Undefined vertex attribute type!");
					}

					pBufferMemory = pBufferMemory + copyAmount;
				}
			}

			aiFace face = {};
			for (UI32 j = 0; j < pMesh->mNumFaces; j++)
			{
				face = pMesh->mFaces[j];
				for (UI32 k = 0; k < face.mNumIndices; k++)
					pIndexes->push_back(face.mIndices[k]);
			}

			const UI64 indexCount = pIndexes->size();
			(*pWireFrame) = WireFrame(pMesh->mName.C_Str(), vertexOffset, pMesh->mNumVertices, 0, indexCount, material);
		}
	}

	const UI64 VertexDescriptor::Size() const
	{
		UI64 size = 0;
		for (const auto attribute : mAttributes)
			size += attribute.mAttributeSize;

		return size;
	}

	AssetLoader::AssetLoader(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::filesystem::path& assetPath, const VertexDescriptor& vertexDescriptor, const bool useMultiThreading)
		: pGeometryStore(pGeometryStore), mDescriptor(vertexDescriptor)
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

		const UI64 vertexSize = vertexDescriptor.Size();
		UI64 vertexCount = 0;

		mWireFrames.resize(pScene->mNumMeshes);
		for (UI32 i = 0; i < pScene->mNumMeshes; i++)
			vertexCount += pScene->mMeshes[i]->mNumVertices;

		const auto pDevice = pGeometryStore->GetDevice();
		const auto pVertexStagingBuffer = pDevice->CreateBuffer(Flint::BufferType::Staging, vertexSize * vertexCount);
		const auto pBufferMemory = static_cast<float*>(pVertexStagingBuffer->MapMemory(vertexSize * vertexCount));

		// Load the mesh data.
		std::vector<std::vector<UI32>> indexData{ pScene->mNumMeshes };

		{
			std::vector<std::future<void>> futures;
			futures.reserve(pScene->mNumMeshes);

			const UI64 vertexFloatCount = vertexDescriptor.Size() / sizeof(float);
			const auto basePath = assetPath.parent_path().string() + LineEnding;

			UI64 vertexOffset = 0;
			for (UI32 i = 0; i < pScene->mNumMeshes; i++)
			{
				const auto pMesh = pScene->mMeshes[i];
				const auto pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

				if (useMultiThreading)
					futures.push_back(std::async(std::launch::async, Helpers::LoadWireFrame, pMesh, pMaterial, basePath, vertexDescriptor, pBufferMemory + (vertexOffset * vertexFloatCount), vertexOffset, &indexData[i], &mWireFrames[i]));
				else
					Helpers::LoadWireFrame(pMesh, pMaterial, basePath, vertexDescriptor, pBufferMemory + (vertexOffset * vertexFloatCount), vertexOffset, &indexData[i], &mWireFrames[i]);

				vertexOffset += pMesh->mNumVertices;
			}
		}

		pVertexStagingBuffer->UnmapMemory();

		UI64 indexCount = 0;
		for (auto& indexes : indexData)
			indexCount += indexes.size();

		const auto pIndexStagingBuffer = pDevice->CreateBuffer(Flint::BufferType::Staging, indexCount * sizeof(UI32));
		const auto pIndexMemory = static_cast<UI32*>(pIndexStagingBuffer->MapMemory(indexCount * sizeof(UI32)));

		UI64 offset = 0;
		for (auto& indexes : indexData)
		{
			std::copy(indexes.begin(), indexes.end(), pIndexMemory + offset);
			offset += indexes.size();
		}

		indexData.clear();
		pIndexStagingBuffer->UnmapMemory();

		const auto [vertexGeometryOffset, indexGeometryOffset] = pGeometryStore->AddGeometry(pVertexStagingBuffer.get(), pIndexStagingBuffer.get());
		UI64 indexOffset = 0;
		for (auto& wireFrame : mWireFrames)
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
			vDescriptor.mAttributes.emplace_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::InputAttributeType::Position));
			vDescriptor.mAttributes.emplace_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::InputAttributeType::ColorZero));
			vDescriptor.mAttributes.emplace_back(Flint::VertexAttribute(sizeof(float) * 2, Flint::InputAttributeType::TextureCoordinatesZero));

			return vDescriptor;
		}

		VertexDescriptor CreateSkyBoxVertexDescriptor()
		{
			Flint::VertexDescriptor vDescriptor = {};
			vDescriptor.mAttributes.emplace_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::InputAttributeType::Position));
			vDescriptor.mAttributes.emplace_back(Flint::VertexAttribute(sizeof(float) * 2, Flint::InputAttributeType::TextureCoordinatesZero));

			return vDescriptor;
		}
	}
}
