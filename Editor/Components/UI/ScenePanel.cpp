// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ScenePanel.hpp"

namespace Flint
{
	ScenePanel::ScenePanel(ImGuiTextureContainer* pContainer)
	{
		ImGui::Begin("Scene Panel");
		ImGui::Image(reinterpret_cast<ImTextureID>(pContainer), ImVec2(848, 480));
	}

	ScenePanel::~ScenePanel()
	{
		ImGui::End();
	}
}