// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/GameObject.hpp"

constexpr UI8 ImageCount = 6;
class TreeScene final : public GameObject
{
public:
	TreeScene(glm::vec3 position, SceneState* pSceneState);
	~TreeScene();

	virtual void OnUpdate(UI64 delta) override final;

private:
	void LoadTreeImages();

private:
	boost::shared_ptr<Flint::Image> pTextures[ImageCount] = {};
	boost::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	boost::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;

	UI64 mVertexOffset = 0, mVertexCount = 0;
	UI64 mIndexOffset = 0, mIndexCount = 0;
	UI64 mDrawIndexes[ImageCount] = {};
};