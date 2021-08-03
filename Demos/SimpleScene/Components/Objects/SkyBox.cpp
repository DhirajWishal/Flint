// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SkyBox.hpp"

#include <glm/gtc/matrix_transform.hpp>

SkyBox::SkyBox(glm::vec3 position, SceneState* pSceneState) : GameObject(position, pSceneState)
{
	mModelMatrix = glm::scale(mModelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));

	pCameraBuffer = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(CameraMatrix));
	pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();

	pVertexShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\SkyBox\\skybox.vert.spv"), Flint::ShaderCodeType::SPIR_V);
	pFragmentShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\SkyBox\\skybox.frag.spv"), Flint::ShaderCodeType::SPIR_V);		

	auto image = LoadSkyboxImages();
	pTexture = pSceneState->pDevice->CreateImage(Flint::ImageType::CUBEMAP, Flint::ImageUsage::GRAPHICS, image.mExtent, Flint::PixelFormat::R8G8B8A8_SRGB, 6, 1, image.pImageData);
	delete[] image.pImageData;

	Flint::ImageSamplerSpecification samplerSpecification = {};
	samplerSpecification.mAddressModeU = Flint::AddressMode::CLAMP_TO_EDGE;
	samplerSpecification.mAddressModeV = Flint::AddressMode::CLAMP_TO_EDGE;
	samplerSpecification.mAddressModeW = Flint::AddressMode::CLAMP_TO_EDGE;
	pTextureSampler = pSceneState->pDevice->CreateImageSampler(samplerSpecification);

	CreateNewPipeline();
	auto pPipeline = pSceneState->pGraphicsPipelines["SkyBox"];
	pResourceMap = pPipeline->CreateResourceMap();

	pResourceMap->SetResource("Ubo", pModelUniform);
	pResourceMap->SetResource("Camera", pCameraBuffer);
	pResourceMap->SetResource("skybox", pTextureSampler, pTexture);

	Flint::FBox2D windowExtent = pSceneState->pDisplay->GetExtent();
	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pDynamicStates->SetScissor(windowExtent, { 0, 0 });

	auto asset = ImportAsset(pSceneState->pDevice, "E:\\Dynamik\\Game Repository\\assets\\assets\\Skybox\\SkySphere.obj");
	auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["Default"]->AddGeometry(asset.pVertexBuffer, asset.pIndexBuffer);
	for (auto instance : asset.mDrawInstances)
		pPipeline->AddDrawData(pResourceMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount);

	pSceneState->pDevice->DestroyBuffer(asset.pVertexBuffer);
	pSceneState->pDevice->DestroyBuffer(asset.pIndexBuffer);
}

SkyBox::~SkyBox()
{
	pSceneState->pDevice->DestroyShader(pVertexShader);
	pSceneState->pDevice->DestroyShader(pFragmentShader);
	pSceneState->pDevice->DestroyBuffer(pCameraBuffer);
	pSceneState->pDevice->DestroyImage(pTexture);
	pSceneState->pDevice->DestroyImageSampler(pTextureSampler);
}

void SkyBox::OnUpdate()
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

void SkyBox::CreateNewPipeline()
{
	Flint::GraphicsPipelineSpecification specification = {};
	specification.mFrontFace = Flint::FrontFace::CLOCKWISE;
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::VIEWPORT | Flint::DynamicStateFlags::SCISSOR;

	pSceneState->pGraphicsPipelines["SkyBox"] = pSceneState->pDevice->CreateGraphicsPipeline("SkyBox", pSceneState->pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitPipeline(pSceneState->pGeometryStores["Default"], pSceneState->pGraphicsPipelines["SkyBox"]);
}

ImageData SkyBox::LoadSkyboxImages()
{
	ImageData images[6] = {};

	images[0] = LoadImage("E:\\Flint\\Assets\\Textures\\SkyBox\\right.jpg");
	images[1] = LoadImage("E:\\Flint\\Assets\\Textures\\SkyBox\\left.jpg");
	images[2] = LoadImage("E:\\Flint\\Assets\\Textures\\SkyBox\\top.jpg");
	images[3] = LoadImage("E:\\Flint\\Assets\\Textures\\SkyBox\\bottom.jpg");
	images[4] = LoadImage("E:\\Flint\\Assets\\Textures\\SkyBox\\front.jpg");
	images[5] = LoadImage("E:\\Flint\\Assets\\Textures\\SkyBox\\back.jpg");

	UI64 imageSize = static_cast<UI64>(images[0].mExtent.mWidth) * images[0].mExtent.mHeight * 4;
	UI64 size = imageSize * 6;

	ImageData finalImage = {};
	finalImage.mExtent = images[0].mExtent;
	finalImage.pImageData = new BYTE[size];

	BYTE* pDataPointer = static_cast<BYTE*>(finalImage.pImageData);
	for (UI8 i = 0; i < 6; i++)
	{
		std::copy(static_cast<BYTE*>(images[i].pImageData), static_cast<BYTE*>(images[i].pImageData) + imageSize, pDataPointer);

		DestroyImage(images[i]);
		pDataPointer += imageSize;
	}

	return finalImage;
}
