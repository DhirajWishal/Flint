// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanStaticModel.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanTexture2D.hpp"

#include "Core/Errors/AssetError.hpp"
#include "Core/Containers/Bytes.hpp"

#include <Optick.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <atomic>
#include <future>
#include <sstream>

namespace /* anonymous */
{
	/**
	 * Static mesh storage structure.
	 * This container temporarily stores all the static mesh's data.
	 */
	struct StaticMeshStorage final
	{
		std::array<std::shared_ptr<Flint::Backend::VulkanBuffer>, AI_MAX_NUMBER_OF_TEXTURECOORDS> m_pTextureBuffers{ nullptr };
		std::array<std::shared_ptr<Flint::Backend::VulkanBuffer>, AI_MAX_NUMBER_OF_COLOR_SETS> m_pColorBuffers{ nullptr };

		std::shared_ptr<Flint::Backend::VulkanBuffer> m_pNormalBuffer = nullptr;
		std::shared_ptr<Flint::Backend::VulkanBuffer> m_pPositionBuffer = nullptr;
		std::shared_ptr<Flint::Backend::VulkanBuffer> m_pTangentBuffer = nullptr;
		std::shared_ptr<Flint::Backend::VulkanBuffer> m_pBiTangentBuffer = nullptr;
	};

	/**
	 * Get the Vulkan format from the attribute.
	 *
	 * @param attribute The attribute to get the format from.
	 * @return The format.
	 */
	VkFormat GetAttributeFormat(Flint::VertexAttribute attribute)
	{
		switch (attribute)
		{
		case Flint::VertexAttribute::Position:
		case Flint::VertexAttribute::Normal:
		case Flint::VertexAttribute::Tangent:
		case Flint::VertexAttribute::BiTangent:
			return VK_FORMAT_R32G32B32_SFLOAT;

		case Flint::VertexAttribute::Color0:
		case Flint::VertexAttribute::Color1:
		case Flint::VertexAttribute::Color2:
		case Flint::VertexAttribute::Color3:
		case Flint::VertexAttribute::Color4:
		case Flint::VertexAttribute::Color5:
		case Flint::VertexAttribute::Color6:
		case Flint::VertexAttribute::Color7:
			return VK_FORMAT_R32G32B32A32_SFLOAT;

		case Flint::VertexAttribute::Texture0:
		case Flint::VertexAttribute::Texture1:
		case Flint::VertexAttribute::Texture2:
		case Flint::VertexAttribute::Texture3:
		case Flint::VertexAttribute::Texture4:
		case Flint::VertexAttribute::Texture5:
		case Flint::VertexAttribute::Texture6:
		case Flint::VertexAttribute::Texture7:
			return VK_FORMAT_R32G32_SFLOAT;

		default:
			throw Flint::BackendError("Invalid attribute type!");
		}
	}

	/**
	 * GEt the texture path from the material.
	 *
	 * @param basePath The base asset path to index the resources from.
	 * @paarm pMaterial The material pointer.
	 * @param type The texture type.
	 * @param index The texture index.
	 * @return The texture path.
	 */
	std::filesystem::path GetTexturePath(const std::filesystem::path& basePath, const aiMaterial* pMaterial, aiTextureType type, uint32_t index)
	{
		OPTICK_EVENT();

		aiString path;
		if (aiReturn_SUCCESS == pMaterial->GetTexture(type, index, &path))
			return basePath / path.C_Str();

		return {};
	}

	/**
	 * Load the static mesh data from the aiMesh.
	 *
	 * @param pMesh The Assimp mesh pointer.
	 * @param pScene th Assimp scene pointer.
	 * @param mesh The mesh to load the data to.
	 * @param meshStorage The mesh storage to load all the data to.
	 * @param device The device to load the data to.
	 * @param vertexOffset The vertex offset of the current mesh.
	 * @param indices The index storage.
	 * @param indicesMutex The mutex used to lock the index storage.
	 * @param basePath The base path to load the assets from.
	 */
	void LoadStaticMesh(const aiMesh* pMesh, const aiScene* pScene, Flint::Backend::StaticMesh& mesh, StaticMeshStorage& meshStorage, Flint::Backend::VulkanDevice& device, uint64_t vertexOffset, std::vector<uint32_t>& indices, std::mutex& indicesMutex, const std::filesystem::path& basePath)
	{
		OPTICK_EVENT();

		mesh.m_Name = pMesh->mName.C_Str();
		mesh.m_VertexCount = pMesh->mNumVertices;
		mesh.m_VertexOffset = vertexOffset;

		// Load the normals if possible.
		if (pMesh->HasNormals())
		{
			auto& normalData = mesh.m_VertexData[EnumToInt(Flint::VertexAttribute::Normal)];

			normalData.m_Stride = sizeof(aiVector3D);
			normalData.m_Size = pMesh->mNumVertices * sizeof(aiVector3D);
			meshStorage.m_pNormalBuffer = std::static_pointer_cast<Flint::Backend::VulkanBuffer>(device.createBuffer(normalData.m_Size, Flint::BufferUsage::Staging, reinterpret_cast<const std::byte*>(pMesh->mNormals)));
		}

		// Load the positions if possible.
		if (pMesh->HasPositions())
		{
			auto& vertexData = mesh.m_VertexData[EnumToInt(Flint::VertexAttribute::Position)];

			vertexData.m_Stride = sizeof(aiVector3D);
			vertexData.m_Size = pMesh->mNumVertices * sizeof(aiVector3D);
			meshStorage.m_pPositionBuffer = std::static_pointer_cast<Flint::Backend::VulkanBuffer>(device.createBuffer(vertexData.m_Size, Flint::BufferUsage::Staging, reinterpret_cast<const std::byte*>(pMesh->mVertices)));
		}

		// Load the tangents and bi-tangents if possible.
		if (pMesh->HasTangentsAndBitangents())
		{
			// Load the tangent data.
			auto& tangentData = mesh.m_VertexData[EnumToInt(Flint::VertexAttribute::Tangent)];

			tangentData.m_Stride = sizeof(aiVector3D);
			tangentData.m_Size = pMesh->mNumVertices * sizeof(aiVector3D);
			meshStorage.m_pTangentBuffer = std::static_pointer_cast<Flint::Backend::VulkanBuffer>(device.createBuffer(tangentData.m_Size, Flint::BufferUsage::Staging, reinterpret_cast<const std::byte*>(pMesh->mTangents)));

			// Load the bi-tangent data.
			auto& biTangentData = mesh.m_VertexData[EnumToInt(Flint::VertexAttribute::BiTangent)];

			biTangentData.m_Stride = sizeof(aiVector3D);
			biTangentData.m_Size = pMesh->mNumVertices * sizeof(aiVector3D);
			meshStorage.m_pBiTangentBuffer = std::static_pointer_cast<Flint::Backend::VulkanBuffer>(device.createBuffer(biTangentData.m_Size, Flint::BufferUsage::Staging, reinterpret_cast<const std::byte*>(pMesh->mBitangents)));
		}

		// Load the texture coordinates if possible.
		for (uint32_t t = 0; t < AI_MAX_NUMBER_OF_TEXTURECOORDS; t++)
		{
			if (pMesh->HasTextureCoords(t))
			{
				auto& textureData = mesh.m_VertexData[EnumToInt(Flint::VertexAttribute::Texture0) + t];

				// We have to do this step to make sure that we are only loading the important 2D data, not the 3D storage.
				std::vector<aiVector2D> textureCoordinates;
				textureCoordinates.reserve(pMesh->mNumVertices);

				std::for_each_n(pMesh->mTextureCoords[t], pMesh->mNumVertices, [&textureCoordinates](const aiVector3D& vec) mutable { textureCoordinates.emplace_back(vec.x, vec.y); });

				textureData.m_Stride = sizeof(aiVector2D);
				textureData.m_Size = pMesh->mNumVertices * sizeof(aiVector2D);
				meshStorage.m_pTextureBuffers[t] = std::static_pointer_cast<Flint::Backend::VulkanBuffer>(device.createBuffer(textureData.m_Size, Flint::BufferUsage::Staging, reinterpret_cast<const std::byte*>(textureCoordinates.data())));
			}
		}

		// Load the vertex colors if possible.
		for (uint32_t c = 0; c < AI_MAX_NUMBER_OF_COLOR_SETS; c++)
		{
			if (pMesh->HasVertexColors(c))
			{
				auto& colorData = mesh.m_VertexData[Flint::EnumToInt(Flint::VertexAttribute::Color0) + c];

				colorData.m_Stride = sizeof(aiColor4D);
				colorData.m_Size = pMesh->mNumVertices * sizeof(aiColor4D);
				meshStorage.m_pColorBuffers[c] = std::static_pointer_cast<Flint::Backend::VulkanBuffer>(device.createBuffer(colorData.m_Size, Flint::BufferUsage::Staging, reinterpret_cast<const std::byte*>(pMesh->mColors[c])));
			}
		}

		// Load the index data if possible.
		if (pMesh->HasFaces())
		{
			[[maybe_unused]] const auto lock = std::scoped_lock(indicesMutex);
			mesh.m_IndexOffset = indices.size();

			for (uint32_t f = 0; f < pMesh->mNumFaces; f++)
			{
				const auto face = pMesh->mFaces[f];

				for (uint32_t index = 0; index < face.mNumIndices; index++)
					indices.emplace_back(face.mIndices[index]);
			}

			mesh.m_IndexCount = indices.size() - mesh.m_IndexOffset;
		}

		// Load the materials.
		const auto pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

		// Get the texture paths.
		mesh.m_TexturePaths[Flint::EnumToInt(Flint::Backend::TextureType::BaseColor)] = GetTexturePath(basePath, pMaterial, AI_MATKEY_BASE_COLOR_TEXTURE);
		mesh.m_TexturePaths[Flint::EnumToInt(Flint::Backend::TextureType::Metalness)] = GetTexturePath(basePath, pMaterial, AI_MATKEY_METALLIC_TEXTURE);
		mesh.m_TexturePaths[Flint::EnumToInt(Flint::Backend::TextureType::Roughness)] = GetTexturePath(basePath, pMaterial, AI_MATKEY_ROUGHNESS_TEXTURE);
		mesh.m_TexturePaths[Flint::EnumToInt(Flint::Backend::TextureType::ColorSheen)] = GetTexturePath(basePath, pMaterial, AI_MATKEY_SHEEN_COLOR_TEXTURE);
		mesh.m_TexturePaths[Flint::EnumToInt(Flint::Backend::TextureType::RoughnessSheen)] = GetTexturePath(basePath, pMaterial, AI_MATKEY_SHEEN_ROUGHNESS_TEXTURE);
		mesh.m_TexturePaths[Flint::EnumToInt(Flint::Backend::TextureType::ColorClearCoat)] = GetTexturePath(basePath, pMaterial, AI_MATKEY_CLEARCOAT_TEXTURE);
		mesh.m_TexturePaths[Flint::EnumToInt(Flint::Backend::TextureType::RoughnessClearCoat)] = GetTexturePath(basePath, pMaterial, AI_MATKEY_CLEARCOAT_ROUGHNESS_TEXTURE);
		mesh.m_TexturePaths[Flint::EnumToInt(Flint::Backend::TextureType::NormalClearCoat)] = GetTexturePath(basePath, pMaterial, AI_MATKEY_CLEARCOAT_NORMAL_TEXTURE);
		mesh.m_TexturePaths[Flint::EnumToInt(Flint::Backend::TextureType::Transmission)] = GetTexturePath(basePath, pMaterial, AI_MATKEY_TRANSMISSION_TEXTURE);
		mesh.m_TexturePaths[Flint::EnumToInt(Flint::Backend::TextureType::VolumeThickness)] = GetTexturePath(basePath, pMaterial, AI_MATKEY_VOLUME_THICKNESS_TEXTURE);
	}

	/**
	 * Save the integer value as bytes.
	 *
	 * @param bytes The bytes to save in.
	 * @param value The value to store.
	 */
	void SaveToBytes(std::vector<std::byte>& bytes, uint64_t value)
	{
		//bytes.emplace_back(value >> 56);
		//bytes.emplace_back(value >> 48);
		//bytes.emplace_back(value >> 40);
		//bytes.emplace_back(value >> 32);
		//bytes.emplace_back(value >> 24);
		//bytes.emplace_back(value >> 16);
		//bytes.emplace_back(value >> 8);
		//bytes.emplace_back(value >> 0);
	}
}

namespace Flint
{
	namespace Backend
	{
		VulkanStaticModel::VulkanStaticModel(const std::shared_ptr<VulkanDevice>& pDevice, std::filesystem::path&& assetFile)
			: StaticModel(pDevice, std::move(assetFile))
			, m_VertexStorage(pDevice)
		{
			OPTICK_EVENT();

			// Load the data from the file.
			loadData();

			// Make sure to set the object as valid.
			validate();
		}

		VulkanStaticModel::~VulkanStaticModel()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanStaticModel::terminate()
		{
			OPTICK_EVENT();

			m_VertexStorage.terminate();
			m_pIndexBuffer->terminate();
			invalidate();
		}

		std::vector<std::byte> VulkanStaticModel::compile() const
		{
			// The format is as follows.
			// Special characters: 'F', 'L', 'I', 'N', 'T'  - 5 bytes.
			// Hash value: 8 bytes.
			// 
			// The next comes the payload.
			// Mesh count: 8 bytes.
			// 
			// Position data size: 8 bytes.
			// Position data.
			// [This format for all the other vertex attributes, same for index data]

			const uint64_t hashValue = 0;
			Bytes bytes;

			// Set the special characters.
			bytes.insert("FLINT");

			// Insert the hash value.
			bytes.insert(hashValue);

			// Insert the mesh count.
			bytes.insert(m_Meshes.size());

			return bytes.getStorage();
		}

		std::vector<VkVertexInputBindingDescription> VulkanStaticModel::getInputBindingDescriptions(const StaticMesh& mesh, const std::vector<VertexInput>& inputs) const
		{
			OPTICK_EVENT();

			std::vector<VkVertexInputBindingDescription> descriptions;

			// Iterate over the vertex data and get the binding descriptions.
			uint32_t binding = 0;
			for (const auto input : inputs)
			{
				const auto i = EnumToInt(input.m_Attribute);
				const auto data = mesh.m_VertexData[i];

				if (data.m_Stride > 0)
				{
					auto& description = descriptions.emplace_back();
					description.stride = data.m_Stride;
					description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
					description.binding = binding++;
				}
			}

			return descriptions;
		}

		std::vector<VkVertexInputAttributeDescription> VulkanStaticModel::getInputAttributeDescriptions(const StaticMesh& mesh, const std::vector<VertexInput>& inputs) const
		{
			OPTICK_EVENT();

			std::vector<VkVertexInputAttributeDescription> descriptions;

			// Iterate over the vertex data and get the binding descriptions.
			uint32_t binding = 0;
			for (const auto input : inputs)
			{
				const auto i = EnumToInt(input.m_Attribute);
				const auto data = mesh.m_VertexData[i];
				if (data.m_Stride > 0)
				{
					auto& description = descriptions.emplace_back();
					description.offset = 0;
					description.format = GetAttributeFormat(input.m_Attribute);
					description.location = i;
					description.binding = binding++;
				}
			}

			return descriptions;
		}

		void VulkanStaticModel::loadData()
		{
			OPTICK_EVENT();

			// Setup the importer and the load the scene.
			Assimp::Importer importer = {};
			const aiScene* pScene = importer.ReadFile(m_AssetPath.string(),
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

			const auto basePath = m_AssetPath.parent_path();

			std::mutex indicesMutex;
			std::vector<uint32_t> indices;
			auto meshStorages = std::vector<StaticMeshStorage>(pScene->mNumMeshes);
			m_Meshes.resize(pScene->mNumMeshes);

			// Load the meshes.
			{
				std::vector<std::future<void>> meshFutures;
				meshFutures.reserve(pScene->mNumMeshes);

				uint64_t vertexOffset = 0;
				for (uint32_t i = 0; i < pScene->mNumMeshes; i++)
				{
					meshFutures.emplace_back(
						std::async(
							std::launch::async,
							[this, pScene, i, vertexOffset, &indices, basePath, &indicesMutex, &meshStorages]
							{
								OPTICK_THREAD("Static Mesh Loader");
								LoadStaticMesh(pScene->mMeshes[i], pScene, m_Meshes[i], meshStorages[i], *getDevice().as<VulkanDevice>(), vertexOffset, indices, indicesMutex, basePath);
							})
					);

					vertexOffset += pScene->mMeshes[i]->mNumVertices;
				}
			}

			// Now we can copy the vertex data.
			for (uint32_t i = 0; i < meshStorages.size(); i++)
			{
				auto& mesh = m_Meshes[i];
				const auto& storage = meshStorages[i];

				mesh.m_VertexData[EnumToInt(VertexAttribute::Position)].m_Offset = m_VertexStorage.insert(VertexAttribute::Position, storage.m_pPositionBuffer.get());
				mesh.m_VertexData[EnumToInt(VertexAttribute::Normal)].m_Offset = m_VertexStorage.insert(VertexAttribute::Normal, storage.m_pNormalBuffer.get());
				mesh.m_VertexData[EnumToInt(VertexAttribute::Tangent)].m_Offset = m_VertexStorage.insert(VertexAttribute::Tangent, storage.m_pTangentBuffer.get());
				mesh.m_VertexData[EnumToInt(VertexAttribute::BiTangent)].m_Offset = m_VertexStorage.insert(VertexAttribute::BiTangent, storage.m_pBiTangentBuffer.get());

				for (uint32_t t = 0; t < AI_MAX_NUMBER_OF_TEXTURECOORDS; t++)
					mesh.m_VertexData[EnumToInt(Flint::VertexAttribute::Texture0) + t].m_Offset = m_VertexStorage.insert(static_cast<Flint::VertexAttribute>(EnumToInt(Flint::VertexAttribute::Texture0) + t), storage.m_pTextureBuffers[t].get());

				for (uint32_t c = 0; c < AI_MAX_NUMBER_OF_COLOR_SETS; c++)
					mesh.m_VertexData[EnumToInt(Flint::VertexAttribute::Color0) + c].m_Offset = m_VertexStorage.insert(static_cast<Flint::VertexAttribute>(EnumToInt(Flint::VertexAttribute::Color0) + c), storage.m_pColorBuffers[c].get());
			}

			// Finally, copy the index data to a staging buffer, and copy it to the final index buffer.
			auto pIndexData = getDevice().createBuffer(indices.size() * sizeof(uint32_t), BufferUsage::Staging, reinterpret_cast<const std::byte*>(indices.data()));

			m_pIndexBuffer = std::static_pointer_cast<VulkanBuffer>(getDevice().createBuffer(pIndexData->getSize(), BufferUsage::Index));
			m_pIndexBuffer->copyFrom(pIndexData.get(), 0, 0);

			// TODO: Export everything to our own optimized binary.
		}
	}
}