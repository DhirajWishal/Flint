// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Camera/Camera.hpp"
#include "Backend.hpp"

namespace Flint
{
	class Engine;

	/**
	 * Scene class.
	 * This represents a single render-able scene with all the attached entities and lighting components.
	 */
	class Scene final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine to which the scene is bound to.
		 * @param camera The camera which is used to render the scene.
		 */
		explicit Scene(Engine& engine, Camera& camera) {}

		/**
		 * Virtual destructor.
		 */
		virtual ~Scene() {}

		/**
		 * Destroy the scene.
		 */
		virtual void destroy() = 0;
	};
}