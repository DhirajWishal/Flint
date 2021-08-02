// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../GameObject.hpp"

class VikingRoom final : public GameObject
{
public:
	VikingRoom(SceneState* pSceneState);
	~VikingRoom();

	virtual void OnUpdate() override final;

private:
	void SetupPipeline();

private:
	std::shared_ptr<Flint::Buffer> pCameraBuffer = nullptr;
	std::shared_ptr<Flint::Image> pTexture = nullptr;
	std::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	std::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	std::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;

	UI64 mVertexOffset = 0, mVertexCount = 0;
	UI64 mIndexOffset = 0, mIndexCount = 0;
	UI64 mDrawIndex = 0;
};