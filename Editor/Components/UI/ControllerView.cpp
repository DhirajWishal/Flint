// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ControllerView.hpp"

#include <imgui.h>

namespace Flint
{
	ControllerView::ControllerView(const std::vector<std::string_view>& controllers)
	{
		ImGui::Begin("Controller View");

		for (const auto identifiers : controllers)
			ImGui::Text(identifiers.data());
	}

	ControllerView::~ControllerView()
	{
		ImGui::End();
	}
}