// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Preview.hpp"

#include <glm/gtc/matrix_transform.hpp>

Preview::Preview(glm::vec3 position, SceneState* pSceneState, std::filesystem::path model, std::vector<std::filesystem::path> texture)
	: GameObject(position, pSceneState)
{
	pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();

	mModelMatrix = glm::rotate(mModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	auto image = LoadImage(texture[0]);
	pTexture = pSceneState->pDevice->CreateImage(Flint::ImageType::DIMENSIONS_2, Flint::ImageUsage::GRAPHICS, image.mExtent, Flint::PixelFormat::R8G8B8A8_SRGB, 1, 1, image.pImageData);
	DestroyImage(image);

	pTextureSampler = pSceneState->pDevice->CreateImageSampler(Flint::ImageSamplerSpecification());

	if (pSceneState->pGraphicsPipelines.find("Default") == pSceneState->pGraphicsPipelines.end())
		pSceneState->CreateDefaultPipeline();

	auto pPipeline = pSceneState->pGraphicsPipelines["Default"];
	pResourceMap = pPipeline->CreateResourceMap();

	pResourceMap->SetResource("Ubo", pModelUniform);
	pResourceMap->SetResource("Camera", pSceneState->mCamera.GetCameraBuffer());
	pResourceMap->SetResource("texSampler", pTextureSampler, pTexture);

	Flint::FBox2D windowExtent = pSceneState->pDisplay->GetExtent();
	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pDynamicStates->SetScissor(windowExtent, { 0, 0 });

	auto asset = ImportAsset(pSceneState->pDevice, model);
	auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["Default"]->AddGeometry(asset.pVertexBuffer, asset.pIndexBuffer);
	for (auto instance : asset.mDrawInstances)
	{
		mDrawIDs.push_back(pPipeline->AddDrawData(pResourceMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount));
		mVertexCount += instance.mVertexCount;
		mIndexCount += instance.mIndexCount;
	}

	mVertexOffset = vertexOffset;
	mIndexOffset = indexOffset;

	pSceneState->pDevice->DestroyBuffer(asset.pVertexBuffer);
	pSceneState->pDevice->DestroyBuffer(asset.pIndexBuffer);
}

Preview::~Preview()
{
	pSceneState->pGeometryStores["Default"]->RemoveGeometry(mVertexOffset, mVertexCount, mIndexOffset, mIndexCount);

	for (const UI64 drawID : mDrawIDs)
		pSceneState->pGraphicsPipelines["Default"]->RemoveDrawData(drawID);
	mDrawIDs.clear();

	pSceneState->pDevice->DestroyImage(pTexture);
	pSceneState->pDevice->DestroyImageSampler(pTextureSampler);
}

void Preview::OnUpdate(UI64 delta)
{
	if (pSceneState->pDisplay->IsDisplayResized())
	{
		auto extent = pSceneState->pDisplay->GetExtent();
		if (!extent.IsZero())
		{
			pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(extent.mWidth), static_cast<float>(extent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
			pDynamicStates->SetScissor(extent, { 0, 0 });
		}
	}

	// Rotate x
	if (pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_X).IsPressed() || pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_X).IsOnRepeat())
	{
		if ((pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_X).GetSpecialCharacter() & Flint::SpecialCharacter::SHIFT) == Flint::SpecialCharacter::SHIFT)
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), mRotationBias, glm::vec3(1.0f, 0.0f, 0.0f));
		else
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), -mRotationBias, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	// Rotate y
	if (pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).IsPressed() || pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).IsOnRepeat())
	{
		if ((pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).GetSpecialCharacter() & Flint::SpecialCharacter::SHIFT) == Flint::SpecialCharacter::SHIFT)
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), mRotationBias, glm::vec3(0.0f, 1.0f, 0.0f));
		else
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), -mRotationBias, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	// Rotate z
	if (pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).IsPressed() || pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).IsOnRepeat())
	{
		if ((pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).GetSpecialCharacter() & Flint::SpecialCharacter::SHIFT) == Flint::SpecialCharacter::SHIFT)
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), mRotationBias, glm::vec3(0.0f, 0.0f, 1.0f));
		else
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), -mRotationBias, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	// Submit data to uniforms.
	SubmitToUniformBuffer(pModelUniform, mModelMatrix);
}
