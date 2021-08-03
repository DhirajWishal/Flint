// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../GameObject.hpp"

class SkyBox final : public GameObject
{
public:
	SkyBox(glm::vec3 position, SceneState* pSceneState);
	~SkyBox();

	virtual void OnUpdate() override final;

private:
	void CreateNewPipeline();
	ImageData LoadSkyboxImages();

private:
	std::shared_ptr<Flint::Buffer> pCameraBuffer = nullptr;
	std::shared_ptr<Flint::Image> pTexture = nullptr;
	std::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	std::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	std::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;
};