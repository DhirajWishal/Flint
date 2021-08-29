// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "AssetLoader.hpp"

#include "Flint/Device.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

boost::container::vector<VertexAttribute> GetDefaultVertexAttributes()
{
	boost::container::vector<VertexAttribute> attributes(3);
	attributes[0] = VertexAttribute(sizeof(glm::vec3), VertexAttributeType::POSITION);
	attributes[1] = VertexAttribute(sizeof(glm::vec3), VertexAttributeType::COLOR_0);
	attributes[2] = VertexAttribute(sizeof(glm::vec2), VertexAttributeType::TEXTURE_COORDINATES_0);

	return attributes;
}

void ProcessNode(const aiNode* pNode, boost::container::vector<Asset::DrawInstance>& drawInstances)
{
	if (!pNode)
		return;

	for (UI32 i = 0; i < pNode->mNumMeshes; i++)
	{
		auto worldPos = pNode->mParent->mTransformation * pNode->mTransformation;

		UI64 index = pNode->mMeshes[i];
		auto& instance = drawInstances[index];

		instance.mTransform[0][0] = worldPos.a1;
		instance.mTransform[0][1] = worldPos.a2;
		instance.mTransform[0][2] = worldPos.a3;
		instance.mTransform[0][3] = worldPos.a4;

		instance.mTransform[1][0] = worldPos.b1;
		instance.mTransform[1][1] = worldPos.b2;
		instance.mTransform[1][2] = worldPos.b3;
		instance.mTransform[1][3] = worldPos.b4;

		instance.mTransform[2][0] = worldPos.c1;
		instance.mTransform[2][1] = worldPos.c2;
		instance.mTransform[2][2] = worldPos.c3;
		instance.mTransform[2][3] = worldPos.c4;

		instance.mTransform[3][0] = worldPos.d1;
		instance.mTransform[3][1] = worldPos.d2;
		instance.mTransform[3][2] = worldPos.d3;
		instance.mTransform[3][3] = worldPos.d4;

		//glm::normalize(drawInstances[index].mTransform);
	}

	for (UI32 i = 0; i < pNode->mNumChildren; i++)
		ProcessNode(pNode->mChildren[i], drawInstances);
}

Asset ImportAsset(const boost::shared_ptr<Flint::Device>& pDevice, const boost::filesystem::path& path, const boost::container::vector<VertexAttribute>& attributes)
{
	Assimp::Importer importer = {};
	const aiScene* pScene = importer.ReadFile(path.string(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_GenUVCoords |
		aiProcess_FlipUVs);

	// Check if the scene could be loaded.
	if (!pScene)
		throw std::runtime_error("Could not open asset file!");

	UI64 vertexSize = 0;
	for (auto& attribute : attributes)
		vertexSize += attribute.mAttributeSize;

	UI64 vertexCount = 0;
	for (UI32 i = 0; i < pScene->mNumMeshes; i++)
		vertexCount += pScene->mMeshes[i]->mNumVertices;

	Asset asset = {};
	asset.pVertexBuffer = pDevice->CreateBuffer(Flint::BufferType::STAGING, vertexCount * vertexSize);
	float* pDataStore = static_cast<float*>(asset.pVertexBuffer->MapMemory(vertexCount * vertexSize));

	UI64 indexCount = 0;
	boost::container::vector<boost::container::vector<UI32>> indexArrays;

	Asset::DrawInstance drawData = {};
	for (UI32 i = 0; i < pScene->mNumMeshes; i++)
	{
		drawData.mVertexOffset += drawData.mVertexCount;
		drawData.mIndexOffset += drawData.mIndexCount;

		auto pMesh = pScene->mMeshes[i];
		auto pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

		boost::container::vector<aiMaterialProperty*> pProperties(pMaterial->mNumProperties);
		std::copy(pMaterial->mProperties, pMaterial->mProperties + pMaterial->mNumProperties, pProperties.begin());

		UI32 countDiff = pMaterial->GetTextureCount(aiTextureType_DIFFUSE);
		UI32 countAmb = pMaterial->GetTextureCount(aiTextureType_AMBIENT);
		UI32 countSpec = pMaterial->GetTextureCount(aiTextureType_SPECULAR);
		UI32 countEmi = pMaterial->GetTextureCount(aiTextureType_EMISSIVE);
		UI32 countTrans = pMaterial->GetTextureCount(aiTextureType_TRANSMISSION);

		aiString string;
		pMaterial->Get(pProperties[0]->mKey.data, pProperties[0]->mType, pProperties[0]->mIndex, string);

		drawData.mName = pMesh->mName.C_Str();
		drawData.mVertexCount = pMesh->mNumVertices;

		for (UI32 j = 0; j < pMesh->mNumVertices; j++)
		{
			for (auto& attribute : attributes)
			{
				UI64 copyAmount = attribute.mAttributeSize / sizeof(float);

				switch (attribute.mType)
				{
				case VertexAttributeType::POSITION:
					if (pMesh->HasPositions())
						std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::NORMAL:
					if (pMesh->HasNormals())
						std::copy(&pMesh->mNormals[j].x, (&pMesh->mNormals[j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::COLOR_0:
					if (pMesh->HasVertexColors(0))
						std::copy(&pMesh->mColors[0][j].r, (&pMesh->mColors[0][j].r) + copyAmount, pDataStore);
					else
						std::fill(pDataStore, pDataStore + (attribute.mAttributeSize / sizeof(float)), 1.0f);
					break;

				case VertexAttributeType::COLOR_1:
					if (pMesh->HasVertexColors(1))
						std::copy(&pMesh->mColors[1][j].r, (&pMesh->mColors[1][j].r) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::COLOR_2:
					if (pMesh->HasVertexColors(2))
						std::copy(&pMesh->mColors[2][j].r, (&pMesh->mColors[1][j].r) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::COLOR_3:
					if (pMesh->HasVertexColors(3))
						std::copy(&pMesh->mColors[3][j].r, (&pMesh->mColors[2][j].r) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::TEXTURE_COORDINATES_0:
					if (pMesh->HasTextureCoords(0))
						std::copy(&pMesh->mTextureCoords[0][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::TEXTURE_COORDINATES_1:
					if (pMesh->HasTextureCoords(1))
						std::copy(&pMesh->mTextureCoords[1][j].x, (&pMesh->mTextureCoords[1][j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::TEXTURE_COORDINATES_2:
					if (pMesh->HasTextureCoords(2))
						std::copy(&pMesh->mTextureCoords[2][j].x, (&pMesh->mTextureCoords[2][j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::TEXTURE_COORDINATES_3:
					if (pMesh->HasTextureCoords(3))
						std::copy(&pMesh->mTextureCoords[3][j].x, (&pMesh->mTextureCoords[3][j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::TEXTURE_COORDINATES_4:
					if (pMesh->HasTextureCoords(4))
						std::copy(&pMesh->mTextureCoords[4][j].x, (&pMesh->mTextureCoords[4][j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::TEXTURE_COORDINATES_5:
					if (pMesh->HasTextureCoords(5))
						std::copy(&pMesh->mTextureCoords[5][j].x, (&pMesh->mTextureCoords[5][j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::TEXTURE_COORDINATES_6:
					if (pMesh->HasTextureCoords(6))
						std::copy(&pMesh->mTextureCoords[6][j].x, (&pMesh->mTextureCoords[6][j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::TEXTURE_COORDINATES_7:
					if (pMesh->HasTextureCoords(7))
						std::copy(&pMesh->mTextureCoords[7][j].x, (&pMesh->mTextureCoords[7][j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::UV_COORDINATES:
					if (pMesh->HasTextureCoords(0))
						std::copy(&pMesh->mTextureCoords[0][j].x, (&pMesh->mTextureCoords[0][j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::TANGENT:
					if (pMesh->HasTangentsAndBitangents())
						std::copy(&pMesh->mTangents[j].x, (&pMesh->mTangents[j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::BITANGENT:
					if (pMesh->HasTangentsAndBitangents())
						std::copy(&pMesh->mBitangents[j].x, (&pMesh->mBitangents[j].x) + copyAmount, pDataStore);
					break;

				case VertexAttributeType::BONE_ID:
					//if (pMesh->HasPositions())
					//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
					//break;

				case VertexAttributeType::BONE_WEIGHT:
					//if (pMesh->HasPositions())
					//	std::copy(&pMesh->mVertices[j].x, (&pMesh->mVertices[j].x) + copyAmount, pDataStore);
					//break;

				case VertexAttributeType::CUSTOM:
					break;

				default:
					throw std::runtime_error("Invalid or Undefined vertex attribute type!");
				}

				pDataStore = pDataStore + (attribute.mAttributeSize / sizeof(float));
			}
		}

		aiFace face = {};
		boost::container::vector<UI32> indexes;
		for (UI32 j = 0; j < pMesh->mNumFaces; j++)
		{
			face = pMesh->mFaces[j];
			for (UI32 k = 0; k < face.mNumIndices; k++)
				indexes.push_back(face.mIndices[k]);
		}
		drawData.mIndexCount = indexes.size();
		indexCount += drawData.mIndexCount;

		INSERT_INTO_VECTOR(indexArrays, std::move(indexes));
		INSERT_INTO_VECTOR(asset.mDrawInstances, drawData);
	}

	asset.pVertexBuffer->UnmapMemory();

	// Create the index buffer.
	asset.pIndexBuffer = pDevice->CreateBuffer(Flint::BufferType::STAGING, indexCount * sizeof(UI32));
	UI32* pIndexDataPointer = static_cast<UI32*>(asset.pIndexBuffer->MapMemory(indexCount * sizeof(UI32)));

	// Copy data to the stagging buffer.
	UI64 offset = 0;
	for (auto itr = indexArrays.begin(); itr != indexArrays.end(); itr++)
	{
		std::copy(itr->begin(), itr->end(), pIndexDataPointer + offset);
		offset += itr->size();
	}
	indexArrays.clear();

	asset.pIndexBuffer->UnmapMemory();

	// Process the nodes.
	ProcessNode(pScene->mRootNode, asset.mDrawInstances);

	return asset;
}

ImageData LoadImage(const boost::filesystem::path& path)
{
	I32 texWidth = 0, texHeight = 0, texChannels = 0;
	stbi_uc* pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	ImageData data;
	data.pImageData = pixels;
	data.mExtent = Flint::FBox3D(texWidth, texHeight, 1);
	return data;
}

void DestroyImage(ImageData data)
{
	stbi_image_free(data.pImageData);
}