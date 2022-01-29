// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/ComponentSystem/ComponentRegistry.hpp"

namespace Flint
{
	class ComponentView
	{
	public:
		ComponentView(const TComponentStore& componentMap);
		~ComponentView();
	};
}
