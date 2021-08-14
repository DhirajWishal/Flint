// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.hpp"
#include "SceneState.hpp"
#include "AssetLoader.hpp"

/**
 * Game object.
 */
class GameObject
{
public:
	GameObject(glm::vec3 position, SceneState* pSceneState);
	virtual ~GameObject();

	/**
	 * This method is called once every new frame.
	 *
	 * @param delta: The time difference.
	 */
	virtual void OnUpdate(UI64 delta) = 0;

public:
	void EnableBoundingBox();
	void UpdateBoundingBox();
	void DisableBoundingBox();

protected:
	template<class Type>
	void SubmitToUniformBuffer(std::shared_ptr<Flint::Buffer> pBuffer, const Type& data)
	{
		Type* pDataStore = static_cast<Type*>(pBuffer->MapMemory(sizeof(Type)));
		*pDataStore = data;

		pBuffer->UnmapMemory();
	}

	void SetupBoundingBox();

protected:
	glm::mat4 mModelMatrix = glm::mat4(1.0f);
	std::shared_ptr<Flint::Buffer> pModelUniform = nullptr;
	std::shared_ptr<Flint::ResourceMap> pResourceMap = nullptr;

	SceneState* pSceneState = nullptr;

	const float mRotationBias = 0.05f;

	glm::vec4 mBoundingBoxColor = {};

private:
	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;
	std::shared_ptr<Flint::ResourceMap> pResourceMapBB = nullptr;
	UI64 mBoundingBoxIndex = 0;
	bool bIsBoundingBoxEnabled = false;
};
