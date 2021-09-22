// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/AssetLoader.hpp"

#include "GraphicsCore/Device.hpp"
#include "GraphicsCore/Material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <tiny_gltf.h>

namespace Flint
{
	const UI64 VertexDescriptor::Size() const
	{
		UI64 size = 0;
		for (const auto attribute : mAttributes)
			size += attribute.mAttributeSize;

		return size;
	}

	AssetLoader::AssetLoader(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::filesystem::path& assetPath, const VertexDescriptor& vertexDescriptor)
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
			aiProcess_SortByPType |
			aiProcess_GenUVCoords |
			aiProcess_FlipUVs);

		// Check if the scene could be loaded.
		if (!pScene)
			throw std::invalid_argument("Provided asset could not be loaded!");

		const UI64 vertexSize = vertexDescriptor.Size();
		UI64 vertexCount = 0;

		for (UI32 i = 0; i < pScene->mNumMeshes; i++)
			vertexCount += pScene->mMeshes[i]->mNumVertices;

		mWireFrames.resize(pScene->mNumMeshes);

		std::shared_ptr<Flint::Buffer> pVertexStagingBuffer = pGeometryStore->GetDevice()->CreateBuffer(Flint::BufferType::Staging, vertexSize * vertexCount);
		float* pBufferMemory = static_cast<float*>(pVertexStagingBuffer->MapMemory(vertexSize * vertexCount));

		// Load the mesh data.
		UI64 vertexOffset = 0, indexOffset = 0;
		std::vector<std::vector<UI32>> indexData{ pScene->mNumMeshes };
		for (UI32 i = 0; i < pScene->mNumMeshes; i++)
		{
			const auto pMesh = pScene->mMeshes[i];
			const auto pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

			std::vector<aiMaterialProperty*> pProperties(pMaterial->mNumProperties);
			std::copy(pMaterial->mProperties, pMaterial->mProperties + pMaterial->mNumProperties, pProperties.begin());

			Material material;

			for (const auto pProperty : pProperties)
			{
				if (pProperty->mType == aiPropertyTypeInfo::aiPTI_Float)
				{
					if (pProperty->mDataLength == (sizeof(float[1])))
					{
						float value = 0;
						pMaterial->Get(pProperty->mKey.C_Str(), pProperty->mType, pProperty->mIndex, value);

						material.AddProperty(std::make_unique<MaterialProperties::Float>(value));
					}
					else if (pProperty->mDataLength == (sizeof(float[2])))
					{
						float value[2] = {};
						pMaterial->Get(pProperty->mKey.C_Str(), pProperty->mType, pProperty->mIndex, value);

						material.AddProperty(std::make_unique<MaterialProperties::Float2>(value));
					}
					else if (pProperty->mDataLength == (sizeof(float[3])))
					{
						float value[3] = {};
						pMaterial->Get(pProperty->mKey.C_Str(), pProperty->mType, pProperty->mIndex, value);

						material.AddProperty(std::make_unique<MaterialProperties::Float3>(value));
					}
					else if (pProperty->mDataLength == (sizeof(float[4])))
					{
						aiColor4D color(0.0f, 0.0f, 0.0f, 0.0f);
						pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);

						float value[4] = { color.r, color.g, color.b, color.a };
						material.AddProperty(std::make_unique<MaterialProperties::Float4>(value));
					}
				}
				else if (pProperty->mType == aiPropertyTypeInfo::aiPTI_Double)
				{
					if (pProperty->mDataLength == (sizeof(double[1])))
					{
						double value = 0;
						pMaterial->Get(pProperty->mKey.C_Str(), pProperty->mType, pProperty->mIndex, value);

						material.AddProperty(std::make_unique<MaterialProperties::Double>(value));
					}
					else if (pProperty->mDataLength == (sizeof(double[2])))
					{
						double value[2] = {};
						pMaterial->Get(pProperty->mKey.C_Str(), pProperty->mType, pProperty->mIndex, value);

						material.AddProperty(std::make_unique<MaterialProperties::Double2>(value));
					}
					else if (pProperty->mDataLength == (sizeof(double[3])))
					{
						double value[3] = {};
						pMaterial->Get(pProperty->mKey.C_Str(), pProperty->mType, pProperty->mIndex, value);

						material.AddProperty(std::make_unique<MaterialProperties::Double3>(value));
					}
					else if (pProperty->mDataLength == (sizeof(double[4])))
					{
						double value[4] = {};
						pMaterial->Get(pProperty->mKey.C_Str(), pProperty->mType, pProperty->mIndex, value);

						material.AddProperty(std::make_unique<MaterialProperties::Double4>(value));
					}
				}
				else if (pProperty->mType == aiPropertyTypeInfo::aiPTI_String)
				{
					if (std::string(pProperty->mKey.C_Str()) == _AI_MATKEY_TEXTURE_BASE)
					{
						aiString path;
						aiReturn result = pMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(pProperty->mIndex), path);

						if (path.length > 0)
							std::filesystem::path texturePath = path.C_Str();
					}
				}
			}

			// Load vertexes.
			for (UI32 j = 0; j < pMesh->mNumVertices; j++)
			{
				for (const auto attribute : vertexDescriptor.mAttributes)
				{
					const UI64 copyAmount = attribute.mAttributeSize / sizeof(float);

					switch (attribute.mType)
					{
					case VertexAttributeType::Position:
						if (pMesh->HasPositions())
							std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::Normal:
						if (pMesh->HasNormals())
							std::copy(&pMesh->mNormals[j].x, (&pMesh->mNormals[j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::ColorZero:
						if (pMesh->HasVertexColors(0))
							std::copy(&pMesh->mColors[0][j].r, (&pMesh->mColors[0][j].r) + copyAmount, pBufferMemory);
						else
							std::fill(pBufferMemory, pBufferMemory + (attribute.mAttributeSize / sizeof(float)), 1.0f);
						break;

					case VertexAttributeType::ColorOne:
						if (pMesh->HasVertexColors(1))
							std::copy(&pMesh->mColors[1][j].r, (&pMesh->mColors[1][j].r) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::ColorTwo:
						if (pMesh->HasVertexColors(2))
							std::copy(&pMesh->mColors[2][j].r, (&pMesh->mColors[1][j].r) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::ColorThree:
						if (pMesh->HasVertexColors(3))
							std::copy(&pMesh->mColors[3][j].r, (&pMesh->mColors[2][j].r) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::TextureCoordinatesZero:
						if (pMesh->HasTextureCoords(0))
							std::copy(&pMesh->mTextureCoords[0][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::TextureCoordinatesOne:
						if (pMesh->HasTextureCoords(1))
							std::copy(&pMesh->mTextureCoords[1][j].x, (&pMesh->mTextureCoords[1][j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::TextureCoordinatesTwo:
						if (pMesh->HasTextureCoords(2))
							std::copy(&pMesh->mTextureCoords[2][j].x, (&pMesh->mTextureCoords[2][j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::TextureCoordinatesThree:
						if (pMesh->HasTextureCoords(3))
							std::copy(&pMesh->mTextureCoords[3][j].x, (&pMesh->mTextureCoords[3][j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::TextureCoordinatesFour:
						if (pMesh->HasTextureCoords(4))
							std::copy(&pMesh->mTextureCoords[4][j].x, (&pMesh->mTextureCoords[4][j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::TextureCoordinatesFive:
						if (pMesh->HasTextureCoords(5))
							std::copy(&pMesh->mTextureCoords[5][j].x, (&pMesh->mTextureCoords[5][j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::TextureCoordinatesSix:
						if (pMesh->HasTextureCoords(6))
							std::copy(&pMesh->mTextureCoords[6][j].x, (&pMesh->mTextureCoords[6][j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::TextureCoordinatesSeven:
						if (pMesh->HasTextureCoords(7))
							std::copy(&pMesh->mTextureCoords[7][j].x, (&pMesh->mTextureCoords[7][j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::UVCoordinates:
						if (pMesh->HasTextureCoords(0))
							std::copy(&pMesh->mTextureCoords[0][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::Tangent:
						if (pMesh->HasTangentsAndBitangents())
							std::copy(&pMesh->mTangents[j].x, (&pMesh->mTangents[j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::Bitangent:
						if (pMesh->HasTangentsAndBitangents())
							std::copy(&pMesh->mBitangents[j].x, (&pMesh->mBitangents[j].x) + copyAmount, pBufferMemory);
						break;

					case VertexAttributeType::BoneID:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pBufferMemory);
						//break;

					case VertexAttributeType::BoneWeight:
						//if (pMesh->HasPositions())
						//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pBufferMemory);
						//break;

					default:
						FLINT_THROW_RUNTIME_ERROR("Invalid or Undefined vertex attribute type!");
					}

					pBufferMemory = pBufferMemory + (attribute.mAttributeSize / sizeof(float));
				}
			}

			aiFace face = {};
			std::vector<UI32> indexes;
			for (UI32 j = 0; j < pMesh->mNumFaces; j++)
			{
				face = pMesh->mFaces[j];
				for (UI32 k = 0; k < face.mNumIndices; k++)
					indexes.push_back(face.mIndices[k]);
			}

			mWireFrames[i] = std::move(WireFrame(pMesh->mName.C_Str(), vertexOffset, pMesh->mNumVertices, indexOffset, indexes.size()));

			vertexOffset += pMesh->mNumVertices;
			indexOffset += indexes.size();

			indexData[i] = std::move(indexes);
		}

		pVertexStagingBuffer->UnmapMemory();

		std::shared_ptr<Flint::Buffer> pIndexStagingBuffer = pGeometryStore->GetDevice()->CreateBuffer(Flint::BufferType::Staging, indexOffset * sizeof(UI32));
		UI32* pIndexMemory = static_cast<UI32*>(pIndexStagingBuffer->MapMemory(indexOffset * sizeof(UI32)));

		UI64 offset = 0;
		for (auto itr = indexData.begin(); itr != indexData.end(); itr++)
			std::copy(itr->begin(), itr->end(), pIndexMemory + offset), offset += itr->size();

		indexData.clear();
		pIndexStagingBuffer->UnmapMemory();

		auto [vertexGeometryOffset, indexGeometryOffset] = pGeometryStore->AddGeometry(pVertexStagingBuffer, pIndexStagingBuffer);

		for (auto& wireFrame : mWireFrames)
		{
			wireFrame.SetVertexOffset(wireFrame.GetVertexOffset() + vertexGeometryOffset);
			wireFrame.SetIndexOffset(wireFrame.GetIndexOffset() + indexGeometryOffset);
		}

		pVertexStagingBuffer->Terminate();
		pIndexStagingBuffer->Terminate();
	}

	namespace Defaults
	{
		VertexDescriptor CreateDefaultVertexDescriptor()
		{
			Flint::VertexDescriptor vDescriptor = {};
			vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::VertexAttributeType::Position));
			vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::VertexAttributeType::ColorZero));
			vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 2, Flint::VertexAttributeType::TextureCoordinatesZero));

			return vDescriptor;
		}
		
		VertexDescriptor CreateSkyBoxVertexDescriptor()
		{
			Flint::VertexDescriptor vDescriptor = {};
			vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::VertexAttributeType::Position));
			vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 2, Flint::VertexAttributeType::TextureCoordinatesZero));

			return vDescriptor;
		}
	}
}
