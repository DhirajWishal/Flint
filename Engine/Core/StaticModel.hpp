// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

#include <glm/glm.hpp>

namespace Flint
{
	namespace Core
	{
		/**
		 * Static mesh structure.
		 */
		struct StaticMesh final
		{
			std::string m_Name;

			uint64_t m_ResourceHash = 0;	// This is used to uniquely identify the mesh's resources when drawing.

			uint64_t m_VertexOffset = 0;	// Bytes
			uint64_t m_IndexOffset = 0;		// Bytes

			uint32_t m_VertexCount = 0;		// Count
			uint32_t m_IndexCount = 0;		// Count
		};

		/**
		 * Static model class.
		 * This contains an asset which does not contain any animations.
		 */
		class StaticModel : public DeviceBoundObject
		{
			/**
			 * Vertex structure.
			 * This structure contains information stored in a single vertex.
			 */
			struct Vertex final
			{
				glm::vec3 m_Position = glm::vec3(1.0f);
				glm::vec3 m_Normal = glm::vec3(1.0f);
				glm::vec2 m_TextureCoordinates = glm::vec2(0.0f);
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device reference.
			 * @param assetFile The asset file to load the data from.
			 */
			explicit StaticModel(const std::shared_ptr<Device>& pDevice, std::filesystem::path&& assetFile) : DeviceBoundObject(pDevice), m_AssetPath(std::move(assetFile)) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~StaticModel() = default;

		protected:
			std::filesystem::path m_AssetPath;

			std::vector<StaticMesh> m_Meshes;
		};
	}
}