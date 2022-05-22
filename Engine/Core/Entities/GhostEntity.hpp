// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Entity.hpp"

namespace Flint
{
	/**
	 * Ghost entity class.
	 * Ghost entities are graphical entities, which does not have a model but can be used to store information such as lighting and other effects.
	 */
	class GhostEntity final : public Entity
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine to which the entity is bound to.
		 */
		explicit GhostEntity(Engine& engine) : Entity(engine) {}
	};
}