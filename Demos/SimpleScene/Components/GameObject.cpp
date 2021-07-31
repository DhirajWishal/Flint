// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GameObject.hpp"

GameObject::GameObject(SceneState* pSceneState) : pSceneState(pSceneState)
{
	pModelUniform = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(glm::mat4));
}

GameObject::~GameObject()
{
	pSceneState->pDevice->DestroyBuffer(pModelUniform);
}
