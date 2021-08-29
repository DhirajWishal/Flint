// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SkyBox.hpp"

#include <glm/gtc/matrix_transform.hpp>

SkyBox::SkyBox(glm::vec3 position, SceneState* pSceneState) : GameObject(position, pSceneState)
{
	const float scale = 50.0f;
	mModelMatrix = glm::scale(mModelMatrix, glm::vec3(scale, scale, scale));

	pDynamicStates = boost::make_shared<Flint::DynamicStateContainer>();

	pVertexShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::VERTEX, boost::filesystem::path("Flint\\Shaders\\SkyBox\\skybox.vert.spv"));
	pFragmentShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::FRAGMENT, boost::filesystem::path("Flint\\Shaders\\SkyBox\\skybox.frag.spv"));

	auto image = LoadSkyboxImages();
	//auto image = LoadImage("C:\\Users\\RLG Evo\\Downloads\\orbita_4k_result.png");
	pTexture = pSceneState->pDevice->CreateImage(Flint::ImageType::CUBEMAP, Flint::ImageUsage::GRAPHICS, image.mExtent, Flint::PixelFormat::R8G8B8A8_SRGB, 6, 1, image.pImageData);
	delete[] image.pImageData;
	//DestroyImage(image);

	Flint::ImageSamplerSpecification samplerSpecification = {};
	samplerSpecification.mAddressModeU = Flint::AddressMode::CLAMP_TO_EDGE;
	samplerSpecification.mAddressModeV = Flint::AddressMode::CLAMP_TO_EDGE;
	samplerSpecification.mAddressModeW = Flint::AddressMode::CLAMP_TO_EDGE;
	pTextureSampler = pSceneState->pDevice->CreateImageSampler(samplerSpecification);

	CreateNewPipeline();
	auto pPipeline = pSceneState->pGraphicsPipelines["SkyBox"];
	pResourceMap = pPipeline->CreateResourceMap();

	pResourceMap->SetResource("Ubo", pModelUniform);
	pResourceMap->SetResource("Camera", pSceneState->mCamera.GetCameraBuffer());
	pResourceMap->SetResource("skybox", pTextureSampler, pTexture);

	Flint::FBox2D windowExtent = pSceneState->pDisplay->GetExtent();
	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pDynamicStates->SetScissor(windowExtent, { 0, 0 });

	auto asset = ImportAsset(pSceneState->pDevice, pSceneState->GetAssetPath().string() + "\\Models\\Skybox\\SkySphere.obj");
	auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["Default"]->AddGeometry(asset.pVertexBuffer, asset.pIndexBuffer);
	for (auto instance : asset.mDrawInstances)
		pPipeline->AddDrawData(pResourceMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount);

	asset.pVertexBuffer->Terminate();
	asset.pIndexBuffer->Terminate();

	//SubmitToUniformBuffer(pModelUniform, mModelMatrix);
}

SkyBox::~SkyBox()
{
	pVertexShader->Terminate();
	pFragmentShader->Terminate();

	pTexture->Terminate();
	pTextureSampler->Terminate();
}

void SkyBox::OnUpdate(UI64 delta)
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

	// Submit data to uniforms.
	SubmitToUniformBuffer(pModelUniform, mModelMatrix);
}

void SkyBox::CreateNewPipeline()
{
	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pSceneState->pDevice->GetSupportedMultiSampleCount();
	specification.mFrontFace = Flint::FrontFace::CLOCKWISE;
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::VIEWPORT | Flint::DynamicStateFlags::SCISSOR;

	pSceneState->pGraphicsPipelines["SkyBox"] = pSceneState->pDevice->CreateGraphicsPipeline("SkyBox", pSceneState->pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitGraphicsPipeline(pSceneState->pGeometryStores["Default"], pSceneState->pGraphicsPipelines["SkyBox"]);
}

ImageData SkyBox::LoadSkyboxImages()
{
	ImageData images[6] = {};

	images[0] = LoadImage(pSceneState->GetAssetPath().string() + "\\Textures\\SkyBox\\right.jpg");
	images[1] = LoadImage(pSceneState->GetAssetPath().string() + "\\Textures\\SkyBox\\left.jpg");
	images[2] = LoadImage(pSceneState->GetAssetPath().string() + "\\Textures\\SkyBox\\top.jpg");
	images[3] = LoadImage(pSceneState->GetAssetPath().string() + "\\Textures\\SkyBox\\bottom.jpg");
	images[4] = LoadImage(pSceneState->GetAssetPath().string() + "\\Textures\\SkyBox\\front.jpg");
	images[5] = LoadImage(pSceneState->GetAssetPath().string() + "\\Textures\\SkyBox\\back.jpg");

	UI64 imageSize = static_cast<UI64>(images[0].mExtent.mWidth) * images[0].mExtent.mHeight * 4;
	const UI64 size = imageSize * 6;

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