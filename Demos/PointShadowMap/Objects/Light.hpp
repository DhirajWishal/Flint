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
	boost::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	boost::shared_ptr<Flint::Shader> pFragmentShader = nullptr;
	boost::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;

	boost::shared_ptr<Flint::Image> pTexture = nullptr;
	boost::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;
};