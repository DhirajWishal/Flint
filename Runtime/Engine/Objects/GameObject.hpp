// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"

namespace Flint
{
	class Scene;

	/**
	 * Game object class.
	 */
	class GameObject
	{
	public:
		/**
		 * Explicit constructor.
		 * 
		 * @param pScene The scene pointer.
		 */
		explicit GameObject(Scene* pScene) : pScene(pScene) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~GameObject() = default;

		/**
		 * On update function.
		 * This function is called once every iteration to update the game object.
		 * 
		 * @param delta The time delta.
		 */
		virtual void OnUpdate(const uint64_t delta) = 0;

		/**
		 * Get the scene object.
		 * 
		 * @return The scene object pointer.
		 */
		Scene* GetScene() { return pScene; }

		/**
		 * Get the scene object.
		 *
		 * @return The scene object pointer.
		 */
		const Scene* GetScene() const { return pScene; }

	protected:
		Scene* pScene = nullptr;
	};
}