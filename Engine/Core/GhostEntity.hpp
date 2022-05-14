// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "entity.hpp"

namespace flint
{
	/**
	 * Ghost entity class.
	 * Ghost entities are graphical entities, which does not have a model but can be used to store information such as lighting and other effects.
	 */
	class ghost_entity final : public entity
	{
	public:
		/**
		 * Default constructor.
		 */
		explicit ghost_entity() = default;
	};
}