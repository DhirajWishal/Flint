// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DrawInstanceUI.hpp"
#include "UIPresets.hpp"

namespace Flint
{
	DrawInstanceUI::DrawInstanceUI(ComponentIdentifier& identifier, const Components::DrawInstanceGraphics& instance)
	{
		ImGui::Begin(std::string("Graphics Draw Instance - " + std::to_string(identifier.mStringHash)).c_str());
	}

	DrawInstanceUI::~DrawInstanceUI()
	{
		ImGui::End();
	}
}