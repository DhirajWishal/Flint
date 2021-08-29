// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/GameObject.hpp"

class VikingRoom final : public GameObject
{
public:
	VikingRoom(glm::vec3 position, SceneState* pSceneState);
	~VikingRoom();

	virtual void OnUpdate(UI64 delta) override final;

private:
	void SetupPipeline();

private:
	boost::shared_ptr<Flint::Image> pTexture = nullptr;
	boost::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	boost::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;

	UI64 mVertexOffset = 0, mVertexCount = 0;
	UI64 mIndexOffset = 0, mIndexCount = 0;
	UI64 mDrawIndex = 0;
};