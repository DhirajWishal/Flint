// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GameObject.hpp"
#include <imgui.h>

class ImGUI : public GameObject
{
	struct PushConstants
	{
		glm::vec2 mScale = glm::vec2(1.0f);
		glm::vec2 mTranslate = glm::vec2(1.0f);
	} pushConstants;

public:
	ImGUI(glm::vec3 position, SceneState* pSceneState);
	~ImGUI();

	virtual void OnUpdate(UI64 delta) override final;

private:
	void PrepareGeometryStore();
	void PreparePipeline();
	void PrepareImage();
	void UpdateBuffers();

private:
	std::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	std::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	std::shared_ptr<Flint::Image> pTextImage = nullptr;
	std::shared_ptr<Flint::ImageSampler> pTextImageSampler = nullptr;

	std::shared_ptr<Flint::ResourceMap> pResourceMap = nullptr;

	std::shared_ptr<Flint::GraphicsPipeline> pPipeline = nullptr;

	std::vector<std::shared_ptr<Flint::DynamicStateContainer>> pDynamicStateContainers;
};
