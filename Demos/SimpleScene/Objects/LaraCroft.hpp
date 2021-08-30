// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/GameObject.hpp"

class LaraCroft final : public GameObject
{
public:
	LaraCroft(glm::vec3 position, SceneState* pSceneState);
	~LaraCroft();

	virtual void OnUpdate(UI64 delta) override final;

private:
	std::shared_ptr<Flint::Buffer> pCameraBuffer = nullptr;
	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;
};