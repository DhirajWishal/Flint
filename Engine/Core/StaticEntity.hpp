// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Entity.hpp"
#include "Geometry.hpp"

#include <filesystem>

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
		 */
		void loadModel(std::filesystem::path&& path);

	private:
		Geometry m_Geometry;
	};
}