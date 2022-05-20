// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "EngineBoundObject.hpp"

namespace Flint
{
	/**
	 * Entity class.
	 */
	class Entity : public EngineBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine to which the entity is bound to.
		 */
		explicit Entity(Engine& engine) : EngineBoundObject(engine) {}
	};
}