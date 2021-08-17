// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/GameObject.hpp"

class VikingRoom final : public GameObject
{
	struct UniformBufferObject
	{
		glm::mat4 mModel = glm::mat4(1.0f);
		I32 mIndex = 1;
	} mUniformBufferObject;

public:
	VikingRoom(glm::vec3 position, SceneState* pSceneState);
	~VikingRoom();

	virtual void OnUpdate(UI64 delta) override final;

private:
	void SetupPipeline();

private:
	std::shared_ptr<Flint::Image> pTexture = nullptr;
	std::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;
	std::shared_ptr<Flint::Buffer> pUniformBufferObject = nullptr;

	std::shared_ptr<Flint::Shader> pVertexShaderMP = nullptr;
	std::shared_ptr<Flint::Shader> pFragmentShaderMP = nullptr;

	UI64 mVertexOffset = 0, mVertexCount = 0;
	UI64 mIndexOffset = 0, mIndexCount = 0;
	UI64 mDrawIndex = 0;
};