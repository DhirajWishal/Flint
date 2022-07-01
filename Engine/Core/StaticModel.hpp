// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Entity.hpp"

namespace Flint
{
	/**
	 * Static mesh class.
	 */
	class StaticMesh final
	{
	public:
		/**
		 * Attribute date array.
		 * This contains the size and offset of each attribute in the buffer.
		 */
		struct AttributeData
		{
			uint8_t m_Stride = 0;	// Bytes.
			uint64_t m_Size = 0;	// Count.
			uint64_t m_Offset = 0;	// Bytes.
		};

	public:
		std::array<AttributeData, EnumToInt(VertexAttribute::Max)> m_VertexData;

		std::string m_Name;

		uint64_t m_ResourceHash = 0;	// This is used to uniquely identify the mesh's resources when drawing.

		uint64_t m_IndexOffset = 0;		// Bytes
		uint64_t m_IndexCount = 0;		// Count
	};

	/**
	 * Static model class.
	 * This contains an asset which does not contain any animations.
	 */
	class StaticModel : public Entity
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device reference.
		 * @param assetFile The asset file to load the data from.
		 */
		explicit StaticModel(const std::shared_ptr<Device>& pDevice, std::filesystem::path&& assetFile) : Entity(pDevice), m_AssetPath(std::move(assetFile)) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~StaticModel() = default;

		/**
		 * Get the meshes from the model.
		 *
		 * @return The loaded meshes.
		 */
		[[nodiscard]] const std::vector<StaticMesh>& getMeshes() const { return m_Meshes; }

	protected:
		const std::filesystem::path m_AssetPath;
		std::vector<StaticMesh> m_Meshes;
	};
}