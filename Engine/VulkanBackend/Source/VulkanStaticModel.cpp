// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanStaticModel.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

#include "Core/Errors/AssetError.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace /* anonymous */
{
	/**
	 * Copy data to the storage.
	 *
	 * @param attribute The type of the attribute data.
	 * @param pStorage The storage pointer to which the data are saved.
	 * @param size The size of the data.
	 * @param pData The data pointer to load the data from.
	 * @return The offset of the data in the storage.
	 */
	uint64_t CopyToStorage(Flint::VertexAttribute attribute, Flint::VulkanBackend::VulkanVertexStorage* pStorage, uint64_t size, const std::byte* pData)
	{
		auto pVertexData = pStorage->getDevice().createBuffer(size, Flint::BufferUsage::Staging, pData);
		return pStorage->insert(attribute, pVertexData.get());
	}

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
}

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanStaticModel::VulkanStaticModel(const std::shared_ptr<VulkanDevice>& pDevice, std::filesystem::path&& assetFile)
			: StaticModel(pDevice, std::move(assetFile))
			, m_VertexStorage(pDevice)
		{
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
			m_VertexStorage.terminate();
			m_pIndexBuffer->terminate();
			invalidate();
		}

		std::vector<VkVertexInputBindingDescription> VulkanStaticModel::getInputBindingDescriptions(const StaticMesh& mesh, const std::vector<VertexInput>& inputs) const
		{
			std::vector<VkVertexInputBindingDescription> descriptions;

			// Iterate over the vertex data and get the binding descriptions.
			uint32_t binding = 0;
			for (const auto input : inputs)
			{
				const auto data = mesh.m_VertexData[EnumToInt(input.m_Attribute)];
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

			std::vector<uint32_t> indices;

			// Load the meshes.
			for (uint32_t i = 0; i < pScene->mNumMeshes; i++)
			{
				const auto pMesh = pScene->mMeshes[i];

				auto& mesh = m_Meshes.emplace_back();
				mesh.m_Name = pMesh->mName.C_Str();

				// Load the normals if possible.
				if (pMesh->HasNormals())
				{
					auto& normalData = mesh.m_VertexData[EnumToInt(VertexAttribute::Normal)];

					normalData.m_Stride = sizeof(aiVector3D);
					normalData.m_Size = pMesh->mNumVertices * sizeof(aiVector3D);
					normalData.m_Offset = CopyToStorage(VertexAttribute::Normal, &m_VertexStorage, normalData.m_Size, reinterpret_cast<const std::byte*>(pMesh->mNormals));
				}

				// Load the positions if possible.
				if (pMesh->HasPositions())
				{
					auto& vertexData = mesh.m_VertexData[EnumToInt(VertexAttribute::Position)];

					vertexData.m_Stride = sizeof(aiVector3D);
					vertexData.m_Size = pMesh->mNumVertices * sizeof(aiVector3D);
					vertexData.m_Offset = CopyToStorage(VertexAttribute::Position, &m_VertexStorage, vertexData.m_Size, reinterpret_cast<const std::byte*>(pMesh->mVertices));
				}

				// Load the tangents and bi-tangents if possible.
				if (pMesh->HasTangentsAndBitangents())
				{
					// Load the tangent data.
					auto& tangentData = mesh.m_VertexData[EnumToInt(VertexAttribute::Tangent)];

					tangentData.m_Stride = sizeof(aiVector3D);
					tangentData.m_Size = pMesh->mNumVertices * sizeof(aiVector3D);
					tangentData.m_Offset = CopyToStorage(VertexAttribute::Tangent, &m_VertexStorage, tangentData.m_Size, reinterpret_cast<const std::byte*>(pMesh->mTangents));

					// Load the bi-tangent data.
					auto& biTangentData = mesh.m_VertexData[EnumToInt(VertexAttribute::BiTangent)];

					biTangentData.m_Stride = sizeof(aiVector3D);
					biTangentData.m_Size = pMesh->mNumVertices * sizeof(aiVector3D);
					biTangentData.m_Offset = CopyToStorage(VertexAttribute::BiTangent, &m_VertexStorage, biTangentData.m_Size, reinterpret_cast<const std::byte*>(pMesh->mBitangents));
				}

				// Load the texture coordinates if possible.
				for (uint32_t t = 0; t < AI_MAX_NUMBER_OF_TEXTURECOORDS; t++)
				{
					if (pMesh->HasTextureCoords(t))
					{
						auto& textureData = mesh.m_VertexData[EnumToInt(VertexAttribute::Texture0) + t];

						textureData.m_Stride = sizeof(aiVector2D);
						textureData.m_Size = pMesh->mNumVertices * sizeof(aiVector3D);
						textureData.m_Offset = CopyToStorage(static_cast<VertexAttribute>(EnumToInt(VertexAttribute::Texture0) + t), &m_VertexStorage, textureData.m_Size, reinterpret_cast<const std::byte*>(pMesh->mTextureCoords[t]));
					}
				}

				// Load the vertex colors if possible.
				for (uint32_t c = 0; c < AI_MAX_NUMBER_OF_COLOR_SETS; c++)
				{
					if (pMesh->HasVertexColors(c))
					{
						auto& colorData = mesh.m_VertexData[EnumToInt(VertexAttribute::Color0) + c];

						colorData.m_Stride = sizeof(aiColor4D);
						colorData.m_Size = pMesh->mNumVertices * sizeof(aiColor4D);
						colorData.m_Offset = CopyToStorage(static_cast<VertexAttribute>(EnumToInt(VertexAttribute::Color0) + c), &m_VertexStorage, colorData.m_Size, reinterpret_cast<const std::byte*>(pMesh->mColors[c]));
					}
				}

				// Load the index data if possible.
				if (pMesh->HasFaces())
				{
					mesh.m_IndexOffset = indices.size() * sizeof(uint32_t);
					mesh.m_IndexCount = indices.size();

					for (uint32_t f = 0; f < pMesh->mNumFaces; f++)
					{
						const auto face = pMesh->mFaces[f];

						for (uint32_t index = 0; index < face.mNumIndices; index++)
							indices.emplace_back(face.mIndices[index]);
					}

					mesh.m_IndexCount = indices.size() - mesh.m_IndexCount;
				}
			}

			// Finally, copy the index data to a staging buffer, and copy it to the final index buffer.
			auto pIndexData = getDevice().createBuffer(indices.size() * sizeof(uint32_t), BufferUsage::Staging, reinterpret_cast<const std::byte*>(indices.data()));
			indices.clear();

			m_pIndexBuffer = std::static_pointer_cast<VulkanBuffer>(getDevice().createBuffer(pIndexData->getSize(), BufferUsage::Index));
			m_pIndexBuffer->copyFrom(pIndexData.get(), 0, 0);
		}
	}
}