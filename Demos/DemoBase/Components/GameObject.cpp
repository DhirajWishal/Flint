// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GameObject.hpp"

#include <glm/gtc/matrix_transform.hpp>

constexpr float Point = 1.0f;

static std::vector<glm::vec3> mBoundingVertexes = {
	glm::vec3(-Point, Point, Point),
	glm::vec3(-Point, Point, -Point),
	glm::vec3(Point, Point, -Point),
	glm::vec3(Point, Point, Point),
	glm::vec3(Point, -Point, Point),
	glm::vec3(Point, -Point, -Point),
	glm::vec3(-Point, -Point, -Point),
	glm::vec3(-Point, -Point, Point)
};

static std::vector<UI32> mBoundingIndexes = {
	0, 1, 1, 2, 2, 3, 3, 0,
	3, 4, 4, 5, 5, 2, 5, 6,
	6, 1, 6, 7, 7, 0, 7, 4
};

GameObject::GameObject(glm::vec3 position, SceneState* pSceneState) : mPosition(position), pSceneState(pSceneState)
{
	pModelUniform = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(glm::mat4));
	mModelMatrix = glm::translate(glm::mat4(1.0f), position);
	//pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();
}

GameObject::~GameObject()
{
	pModelUniform->Terminate();
}

void GameObject::EnableBoundingBox()
{
	if (bIsBoundingBoxEnabled)
		return;

	mBoundingBoxIndex = pSceneState->pGraphicsPipelines["BoundingBox"]->AddDrawData(pResourceMapBB, pDynamicStates, 0, mBoundingVertexes.size(), 0, mBoundingIndexes.size());
	bIsBoundingBoxEnabled = true;
}

void GameObject::UpdateBoundingBox()
{
	//pDynamicStates->SetConstantData(Flint::ShaderType::FRAGMENT, &mBoundingBoxColor, sizeof(glm::vec4));
}

void GameObject::DisableBoundingBox()
{
	if (!bIsBoundingBoxEnabled)
		return;

	pSceneState->pGraphicsPipelines["BoundingBox"]->RemoveDrawData(mBoundingBoxIndex);
	bIsBoundingBoxEnabled = false;
}

void GameObject::SetupBoundingBox()
{
	if (pSceneState->pGeometryStores.find("BoundingBox") == pSceneState->pGeometryStores.end())
	{
		pSceneState->pGeometryStores["BoundingBox"] = pSceneState->pDevice->CreateGeometryStore(pSceneState->pVertexShaderBB->GetInputAttributes(), sizeof(UI32));
		auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["BoundingBox"]->AddGeometry(mBoundingVertexes.size(), mBoundingVertexes.data(), mBoundingIndexes.size(), mBoundingIndexes.data());
		pSceneState->pScreenBoundRenderTargets["Default"]->SubmitPipeline(pSceneState->pGeometryStores["BoundingBox"], pSceneState->pGraphicsPipelines["BoundingBox"]);
	}

	pResourceMapBB = pSceneState->pGraphicsPipelines["BoundingBox"]->CreateResourceMap();
	pResourceMapBB->SetResource("camera", pSceneState->mCamera.GetCameraBuffer());
	pResourceMapBB->SetResource("Ubo", pModelUniform);
}
