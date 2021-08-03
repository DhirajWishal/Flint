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
	 * @param pCamera: The camera pointer.
	 * @param pDisplay: The display pointer.
	 */
	virtual void OnUpdate() = 0;

protected:
	template<class Type>
	void SubmitToUniformBuffer(std::shared_ptr<Flint::Buffer> pBuffer, const Type& data)
	{
		Type* pDataStore = static_cast<Type*>(pBuffer->MapMemory(sizeof(Type)));
		*pDataStore = data;

		pBuffer->UnmapMemory();
	}

protected:
	glm::mat4 mModelMatrix = glm::mat4(1.0f);
	std::shared_ptr<Flint::Buffer> pModelUniform = nullptr;
	std::shared_ptr<Flint::ResourceMap> pResourceMap = nullptr;

	SceneState* pSceneState = nullptr;

	const float mRotationBias = 0.005f;
};
