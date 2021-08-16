// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GameObject.hpp"

class ImGuizmo : public GameObject
{
public:
	ImGuizmo(glm::vec3 position, SceneState* pSceneState);
	~ImGuizmo();

	virtual void OnUpdate(UI64 delta) override final;
};