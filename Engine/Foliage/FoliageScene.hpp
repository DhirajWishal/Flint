// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Scene.hpp"

#include <memory>

namespace Flint
{
	/**
	 * Foliage scene class.
	 * This class is used to render realistic looking graphics using either ray tracing or rasterization.
	 */
	class FoliageScene final : public std::enable_shared_from_this<FoliageScene>, public Scene
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pEngine The engine to which the scene is bound to.
		 * @param camera The camera which is used to render the scene.
		 */
		explicit FoliageScene(const std::shared_ptr<Engine>& pEngine, Camera& camera) : Scene(pEngine, camera) {}

		/**
		 * Virtual destructor.
		 */
		~FoliageScene() override {}
	};
}