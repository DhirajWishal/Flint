// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LaraCroft.hpp"

LaraCroft::LaraCroft(glm::vec3 position, SceneState* pSceneState)
	: GameObject(position, pSceneState)
{
	pCameraBuffer = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(CameraMatrix));
	pDynamicStates = boost::make_shared<Flint::DynamicStateContainer>();

	auto asset = ImportAsset(pSceneState->pDevice, "E:\\Dynamik\\Game Repository\\assets\\assets\\lara croft\\source\\only head for sketchfab.obj");
}

LaraCroft::~LaraCroft()
{
}

void LaraCroft::OnUpdate(UI64 delta)
{
}