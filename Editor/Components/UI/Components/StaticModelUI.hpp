// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/ComponentSystem/ComponentRegistry.hpp"
#include "Engine/Components/StaticModel.hpp"

namespace Flint
{
	class StaticModelUI
	{
	public:
		StaticModelUI(ComponentIdentifier& identifier, const Components::StaticModel& model);
		~StaticModelUI();
	};
}
