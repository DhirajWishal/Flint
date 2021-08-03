// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SkyBox.hpp"

class Reflection final : public GameObject
{
public:
	Reflection(glm::vec3 position, SceneState* pSceneState, SkyBox* pSkyBox);
	~Reflection();

	virtual void OnUpdate() override final;

private:
	std::shared_ptr<Flint::Buffer> pCameraBuffer = nullptr;

	std::shared_ptr<Flint::Image> pTexture = nullptr;
	std::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;

	UI64 mVertexOffset = 0, mVertexCount = 0;
	UI64 mIndexOffset = 0, mIndexCount = 0;
	UI64 mDrawIndexes = {};
};
