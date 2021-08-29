// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Reflection.hpp"

Reflection::Reflection(glm::vec3 position, SceneState* pSceneState, SkyBox* pSkyBox)
	: GameObject(position, pSceneState)
{
	pCameraBuffer = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(CameraMatrix));
	pDynamicStates = boost::make_shared<Flint::DynamicStateContainer>();

	pTextureSampler = pSceneState->pDevice->CreateImageSampler(Flint::ImageSamplerSpecification());
}

Reflection::~Reflection()
{
}

void Reflection::OnUpdate(UI64 delta)
{
}