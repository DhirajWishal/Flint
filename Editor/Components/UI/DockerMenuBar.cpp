// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DockerMenuBar.hpp"
#include "Core/DataType.hpp"

namespace Flint
{
	DockerMenuBar::DockerMenuBar(float frameTime)
	{
		ImGui::BeginMenuBar();
	}

	DockerMenuBar::~DockerMenuBar()
	{
		ImGui::EndMenuBar();
	}
}