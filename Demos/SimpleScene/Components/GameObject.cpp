// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GameObject.hpp"

#include <glm/gtc/matrix_transform.hpp>

GameObject::GameObject(glm::vec3 position, SceneState* pSceneState) : pSceneState(pSceneState)
{
	pModelUniform = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(glm::mat4));
	mModelMatrix = glm::translate(glm::mat4(1.0f), position);
}

GameObject::~GameObject()
{
	pSceneState->pDevice->DestroyBuffer(pModelUniform);
}
