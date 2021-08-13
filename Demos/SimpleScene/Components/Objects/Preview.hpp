// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../GameObject.hpp"

class Preview final : public GameObject
{
	struct Light
	{
		glm::mat4 mLightSpace = glm::mat4(1.0f);
		glm::vec3 mLightPosition = glm::vec3(1.0f);
	} mLight;

public:
	Preview(glm::vec3 position, SceneState* pSceneState, std::filesystem::path model, std::vector<std::filesystem::path> textures);
	~Preview();

	virtual void OnUpdate(UI64 delta) override final;

private:
	void PrepareNoTexturePipeline();
	void PrepareShadowMapPipeline();

private:
	std::shared_ptr<Flint::Image> pTexture = nullptr;
	std::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;
	std::shared_ptr<Flint::DynamicStateContainer> pShadowDynamicStates = nullptr;

	std::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	std::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	std::shared_ptr<Flint::Shader> pShadowVertexShader = nullptr;
	std::shared_ptr<Flint::Shader> pShadowFragmentShader = nullptr;

	UI64 mVertexOffset = 0, mVertexCount = 0;
	UI64 mIndexOffset = 0, mIndexCount = 0;
	std::vector<UI64> mDrawIDs;
	std::vector<std::shared_ptr<Flint::Buffer>> pModelMatrixes;
	std::shared_ptr<Flint::Buffer> pLightUniform;
	std::shared_ptr<Flint::Buffer> pShadowMapUniform;
	std::shared_ptr<Flint::Buffer> pLightObject;
};