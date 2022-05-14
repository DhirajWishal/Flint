// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Entity.hpp"

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
		 * Default constructor.
		 */
		constexpr StaticEntity() = default;

		/**
		 * Load the static entity from an asset file.
		 *
		 * @param path The asset path.
		 * @throws AssetError If the specified asset could not be loaded.
		 */
		void loadModel(std::filesystem::path&& path);
	};
}