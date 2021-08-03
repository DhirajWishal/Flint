// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "TreeScene.hpp"

#include <glm/gtc/matrix_transform.hpp>

TreeScene::TreeScene(glm::vec3 position, SceneState* pSceneState)
	: GameObject(position, pSceneState)
{
	pCameraBuffer = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(CameraMatrix));
	pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();

	pTextureSampler = pSceneState->pDevice->CreateImageSampler(Flint::ImageSamplerSpecification());

	Flint::FBox2D windowExtent = pSceneState->pDisplay->GetExtent();
	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pDynamicStates->SetScissor(windowExtent, { 0, 0 });

	if (pSceneState->pGraphicsPipelines.find("Default") == pSceneState->pGraphicsPipelines.end())
		pSceneState->CreateDefaultPipeline();

	auto asset = ImportAsset(pSceneState->pDevice, "E:\\Dynamik\\Game Repository\\assets\\assets\\Tree01\\Tree1\\Tree1.obj");
	auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["Default"]->AddGeometry(asset.pVertexBuffer, asset.pIndexBuffer);

	LoadTreeImages();

	auto pPipeline = pSceneState->pGraphicsPipelines["Default"];
	for (UI8 i = 0; i < ImageCount; i++)
	{
		const auto instance = asset.mDrawInstances[i];

		auto pMap = pPipeline->CreateResourceMap();
		pMap->SetResource("Ubo", pModelUniform);
		pMap->SetResource("Camera", pCameraBuffer);
		pMap->SetResource("texSampler", pTextureSampler, pTextures[i]);

		mDrawIndexes[i] = pPipeline->AddDrawData(pMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount);
		
		mVertexCount += instance.mVertexCount;
		mIndexCount += instance.mIndexCount;
	}

	mVertexOffset = vertexOffset;
	mIndexOffset = indexOffset;

	pSceneState->pDevice->DestroyBuffer(asset.pVertexBuffer);
	pSceneState->pDevice->DestroyBuffer(asset.pIndexBuffer);
}

TreeScene::~TreeScene()
{
	pSceneState->pGeometryStores["Default"]->RemoveGeometry(mVertexOffset, mVertexCount, mIndexOffset, mIndexCount);

	for (UI8 i = 0; i < ImageCount; i++)
		pSceneState->pGraphicsPipelines["Default"]->RemoveDrawData(mDrawIndexes[i]);

	pSceneState->pDevice->DestroyBuffer(pCameraBuffer);
	pSceneState->pDevice->DestroyImageSampler(pTextureSampler);

	for (UI8 i = 0; i < ImageCount; i++)
		pSceneState->pDevice->DestroyImage(pTextures[i]);
}

void TreeScene::OnUpdate()
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
	SubmitToUniformBuffer(pCameraBuffer, pSceneState->mCamera.GetMatrix());
}

void TreeScene::LoadTreeImages()
{
	std::filesystem::path paths[ImageCount] = {
		"E:\\Dynamik\\Game Repository\\assets\\assets\\Tree01\\Tree1\\BarkDecidious0143_5_S.jpg",
		"E:\\Dynamik\\Game Repository\\assets\\assets\\Tree01\\Tree1\\Leaves0120_35_S.png",
		"E:\\Dynamik\\Game Repository\\assets\\assets\\Tree01\\Tree1\\BarkDecidious0194_7_S.jpg",
		"E:\\Dynamik\\Game Repository\\assets\\assets\\Tree01\\Tree1\\Leaves0142_4_S.png",
		"E:\\Dynamik\\Game Repository\\assets\\assets\\Tree01\\Tree1\\BarkDecidious0194_7_S.jpg",
		"E:\\Dynamik\\Game Repository\\assets\\assets\\Tree01\\Tree1\\Leaves0156_1_S.png",
		//"E:\\Dynamik\\Game Repository\\assets\\Images\\spring-forrest-i101995.jpg"
	};

	for (UI8 i = 0; i < ImageCount; i++)
	{
		auto image = LoadImage(paths[i]);
		pTextures[i] = pSceneState->pDevice->CreateImage(Flint::ImageType::DIMENSIONS_2, Flint::ImageUsage::GRAPHICS, image.mExtent, Flint::PixelFormat::R8G8B8A8_SRGB, 1, 1, image.pImageData);
		delete[] image.pImageData;
	}
}
