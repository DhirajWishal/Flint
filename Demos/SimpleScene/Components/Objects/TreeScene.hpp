// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../GameObject.hpp"

constexpr UI8 ImageCount = 6;
class TreeScene final : public GameObject
{
public:
	TreeScene(glm::vec3 position, SceneState* pSceneState);
	~TreeScene();

	virtual void OnUpdate() override final;

private:
	void LoadTreeImages();

private:
	std::shared_ptr<Flint::Buffer> pCameraBuffer = nullptr;

	std::shared_ptr<Flint::Image> pTextures[ImageCount] = {};
	std::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;

	UI64 mVertexOffset = 0, mVertexCount = 0;
	UI64 mIndexOffset = 0, mIndexCount = 0;
	UI64 mDrawIndexes[ImageCount] = {};
};
