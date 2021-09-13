// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/ComponentSystem/ComponentRegistry.hpp"
#include "Engine/Components/DrawInstance.hpp"

namespace Flint
{
	class DrawInstanceUI
	{
	public:
		DrawInstanceUI(ComponentIdentifier& identifier, const Components::DrawInstanceGraphics& instance);
		~DrawInstanceUI();
	};
}
