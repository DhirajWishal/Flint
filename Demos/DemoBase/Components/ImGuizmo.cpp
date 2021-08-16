// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImGuizmo.hpp"

ImGuizmo::ImGuizmo(glm::vec3 position, SceneState* pSceneState)
	: GameObject(position, pSceneState)
{
}

ImGuizmo::~ImGuizmo()
{
}

void ImGuizmo::OnUpdate(UI64 delta)
{
}