// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Entity.hpp"
#include "Core/Loader/GeometryLoader.hpp"

namespace Flint
{
	/**
	 * Static entity class.
	 * This class contains all the information about a simple model, and how it should be rendered.
	 */
	class StaticEntity final : public Entity
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine to which the entity is bound to.
		 */
		explicit StaticEntity(Engine& engine) : Entity(engine) {}

		/**
		 * Load the static entity from an asset file.
		 *
		 * @param path The asset path.
		 * @param loadData The vertex data to load. Default is all.
		 */
		void loadModel(std::filesystem::path&& path, VertexData loadData = VertexData::All);

		/**
		 * Get the stored geometry.
		 *
		 * @return The geometry.
		 */
		[[nodiscard]] const Geometry& getGeometry() const { return m_Geometry; }

	private:
		Geometry m_Geometry;
	};
}