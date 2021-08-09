// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VikingRoom.hpp"

#include <glm/gtc/matrix_transform.hpp>

VikingRoom::VikingRoom(glm::vec3 position, SceneState* pSceneState) : GameObject(position, pSceneState)
{
	pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();

	auto image = LoadImage(pSceneState->GetAssetPath().string() + "\\Packages\\VikingRoom\\VikingRoom\\texture.png");
	pTexture = pSceneState->pDevice->CreateImage(Flint::ImageType::DIMENSIONS_2, Flint::ImageUsage::GRAPHICS, image.mExtent, Flint::PixelFormat::R8G8B8A8_SRGB, 1, 1, image.pImageData);
	DestroyImage(image);

	pTextureSampler = pSceneState->pDevice->CreateImageSampler(Flint::ImageSamplerSpecification());

	if (pSceneState->pGraphicsPipelines.find("DefaultWireframe") == pSceneState->pGraphicsPipelines.end())
		SetupPipeline();

	auto pPipeline = pSceneState->pGraphicsPipelines["DefaultWireframe"];
	pResourceMap = pPipeline->CreateResourceMap();

	pResourceMap->SetResource("Ubo", pModelUniform);
	pResourceMap->SetResource("Camera", pSceneState->mCamera.GetCameraBuffer());
	pResourceMap->SetResource("texSampler", pTextureSampler, pTexture);

	Flint::FBox2D windowExtent = pSceneState->pDisplay->GetExtent();
	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pDynamicStates->SetScissor(windowExtent, { 0, 0 });

	auto asset = ImportAsset(pSceneState->pDevice, pSceneState->GetAssetPath().string() + "\\Packages\\VikingRoom\\VikingRoom\\vikingroom.fbx");
	auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["Default"]->AddGeometry(asset.pVertexBuffer, asset.pIndexBuffer);
	for (auto instance : asset.mDrawInstances)
	{
		mDrawIndex = pPipeline->AddDrawData(pResourceMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount);
		mVertexCount += instance.mVertexCount;
		mIndexCount += instance.mIndexCount;
	}

	mVertexOffset = vertexOffset;
	mIndexOffset = indexOffset;

	pSceneState->pDevice->DestroyBuffer(asset.pVertexBuffer);
	pSceneState->pDevice->DestroyBuffer(asset.pIndexBuffer);
	SetupBoundingBox();
}

VikingRoom::~VikingRoom()
{
	pSceneState->pGeometryStores["Default"]->RemoveGeometry(mVertexOffset, mVertexCount, mIndexOffset, mIndexCount);
	pSceneState->pGraphicsPipelines["DefaultWireframe"]->RemoveDrawData(mDrawIndex);

	pSceneState->pDevice->DestroyImage(pTexture);
	pSceneState->pDevice->DestroyImageSampler(pTextureSampler);
}

void VikingRoom::OnUpdate(UI64 delta)
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
	UpdateBoundingBox();
}

void VikingRoom::SetupPipeline()
{
	Flint::GraphicsPipelineSpecification specification = {};
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::VIEWPORT | Flint::DynamicStateFlags::SCISSOR;
	specification.bEnableDepthTest = true;
	specification.bEnableDepthWrite = true;
	specification.mColorBlendConstants[0] = 0.0f;
	specification.mColorBlendConstants[1] = 0.0f;
	specification.mColorBlendConstants[2] = 0.0f;
	specification.mColorBlendConstants[3] = 0.0f;
	//specification.mPrimitiveTopology = Flint::PrimitiveTopology::TRIANGLE_LIST;
	//specification.mPolygonMode = Flint::PolygonMode::LINE;

	pSceneState->pGraphicsPipelines["DefaultWireframe"] = pSceneState->pDevice->CreateGraphicsPipeline("DefaultWireframe", pSceneState->pScreenBoundRenderTargets["Default"], pSceneState->pVertexShader, nullptr, nullptr, nullptr, pSceneState->pFragmentShader, specification);
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitPipeline(pSceneState->pGeometryStores["Default"], pSceneState->pGraphicsPipelines["DefaultWireframe"]);
}
