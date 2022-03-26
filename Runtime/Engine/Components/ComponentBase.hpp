// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"

namespace Flint
{
	namespace Components
	{
		/**
		 * Component base structure.
		 */
		struct ComponentBase
		{
			ComponentBase(const std::string_view& name) : mComponentName(name) {}

			std::string_view mComponentName = "";
		};
	}
}