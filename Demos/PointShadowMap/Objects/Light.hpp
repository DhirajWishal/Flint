// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/GameObject.hpp"

class Light : public GameObject
{
public:
	Light(glm::vec3 position, SceneState* pSceneState);
	~Light();

	virtual void OnUpdate(UI64 delta) override;

private:
	std::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	std::shared_ptr<Flint::Shader> pFragmentShader = nullptr;
	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;

	std::shared_ptr<Flint::Image> pTexture = nullptr;
	std::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;
};