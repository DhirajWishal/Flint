// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../GameObject.hpp"

class LaraCroft final : public GameObject
{
public:
	LaraCroft(glm::vec3 position, SceneState* pSceneState);
	~LaraCroft();

	virtual void OnUpdate() override final;

private:
	std::shared_ptr<Flint::Buffer> pCameraBuffer = nullptr;
	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;
};