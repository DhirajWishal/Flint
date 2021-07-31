// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SkyBox.hpp"

#include <glm/gtc/matrix_transform.hpp>

SkyBox::SkyBox(SceneState* pSceneState) : GameObject(pSceneState)
{
	pCameraBuffer = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(CameraMatrix));
	pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();

	pVertexShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\SkyBox\\skybox.vert.spv"), Flint::ShaderCodeType::SPIR_V);
	pFragmentShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\SkyBox\\skybox.frag.spv"), Flint::ShaderCodeType::SPIR_V);

	if (pSceneState->pGeometryStores.find("Default") == pSceneState->pGeometryStores.end())
		pSceneState->pGeometryStores["Default"] = pSceneState->pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI32));

	auto [vertices, indices] = LoadAsset("E:\\Dynamik\\Game Repository\\assets\\assets\\Skybox\\SkySphere.obj");
	auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["Default"]->AddGeometry(vertices.size(), vertices.data(), indices.size(), indices.data());

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

	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(pSceneState->mDisplayExtent.mWidth), static_cast<float>(pSceneState->mDisplayExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pDynamicStates->SetScissor(pSceneState->mDisplayExtent, { 0, 0 });
	pPipeline->AddDrawData(pResourceMap, pDynamicStates, vertexOffset, vertices.size(), indexOffset, indices.size());
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
		pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(pSceneState->pDisplay->GetExtent().mWidth), static_cast<float>(pSceneState->pDisplay->GetExtent().mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
		pDynamicStates->SetScissor(pSceneState->pDisplay->GetExtent(), { 0, 0 });
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
