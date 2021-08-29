// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/GameObject.hpp"

class Preview final : public GameObject
{
	struct Light
	{
		glm::mat4 mLightSpace = glm::mat4(1.0f);
		glm::vec3 mLightPosition = glm::vec3(1.0f);
	} mLight;

public:
	Preview(glm::vec3 position, SceneState* pSceneState, boost::filesystem::path model, boost::container::vector<boost::filesystem::path> textures);
	~Preview();

	virtual void OnUpdate(UI64 delta) override final;

private:
	void PrepareNoTexturePipeline();
	void PrepareShadowMapPipeline();

private:
	boost::shared_ptr<Flint::ImageSampler> pShadowSampler = nullptr;
	boost::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;
	boost::shared_ptr<Flint::Image> pTexture = nullptr;

	boost::container::vector<boost::shared_ptr<Flint::ImageSampler>> pTextureSamplers;
	boost::container::vector<boost::shared_ptr<Flint::Image>> pTextures;

	boost::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;
	boost::shared_ptr<Flint::DynamicStateContainer> pShadowDynamicStates = nullptr;

	boost::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	boost::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	boost::shared_ptr<Flint::Shader> pShadowVertexShader = nullptr;
	boost::shared_ptr<Flint::Shader> pShadowFragmentShader = nullptr;

	UI64 mVertexOffset = 0, mVertexCount = 0;
	UI64 mIndexOffset = 0, mIndexCount = 0;
	boost::container::vector<UI64> mDrawIDs;
	boost::container::vector<boost::shared_ptr<Flint::Buffer>> pModelMatrixes;
	boost::shared_ptr<Flint::Buffer> pLightUniform;
	boost::shared_ptr<Flint::Buffer> pShadowMapUniform;
	boost::shared_ptr<Flint::Buffer> pLightObject;
};