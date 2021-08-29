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
	boost::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	boost::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	boost::shared_ptr<Flint::Image> pTextImage = nullptr;
	boost::shared_ptr<Flint::ImageSampler> pTextImageSampler = nullptr;

	boost::shared_ptr<Flint::ResourceMap> pResourceMap = nullptr;

	boost::shared_ptr<Flint::GraphicsPipeline> pPipeline = nullptr;

	boost::container::vector<boost::shared_ptr<Flint::DynamicStateContainer>> pDynamicStateContainers;
};