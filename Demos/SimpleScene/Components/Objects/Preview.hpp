// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../GameObject.hpp"

class Preview final : public GameObject
{
public:
	Preview(glm::vec3 position, SceneState* pSceneState, std::filesystem::path model, std::filesystem::path texture);
	~Preview();

	virtual void OnUpdate(UI64 delta) override final;

private:
	std::shared_ptr<Flint::Image> pTexture = nullptr;
	std::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;

	UI64 mVertexOffset = 0, mVertexCount = 0;
	UI64 mIndexOffset = 0, mIndexCount = 0;
	std::vector<UI64> mDrawIDs;
};