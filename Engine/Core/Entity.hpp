// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.hpp"

namespace Flint
{
	/**
	 * Entity class.
	 */
	class Entity : public FObject
	{
	public:
		/**
		 * Default constructor.
		 */
		constexpr Entity() = default;
	};
}