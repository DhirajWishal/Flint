// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/GameObject.hpp"

class SkyBox final : public GameObject
{
public:
	SkyBox(glm::vec3 position, SceneState* pSceneState);
	~SkyBox();

	virtual void OnUpdate(UI64 delta) override final;

private:
	void CreateNewPipeline();
	ImageData LoadSkyboxImages();

private:
	boost::shared_ptr<Flint::Image> pTexture = nullptr;
	boost::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	boost::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	boost::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	boost::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;
};