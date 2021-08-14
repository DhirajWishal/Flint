// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Preview.hpp"

#include <glm/gtc/matrix_transform.hpp>

Preview::Preview(glm::vec3 position, SceneState* pSceneState, std::filesystem::path model, std::vector<std::filesystem::path> texture)
	: GameObject(position, pSceneState)
{
	pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();
	pShadowDynamicStates = std::make_shared<Flint::DynamicStateContainer>();

	pSceneState->mCamera.SetCameraRange(1.0f, 100.0f);

	mModelMatrix = glm::rotate(mModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	pLightUniform = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(Light));
	pShadowMapUniform = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(glm::mat4));

	std::vector<VertexAttribute> attributes(4);
	attributes[0] = VertexAttribute(sizeof(glm::vec3), VertexAttributeType::POSITION);
	attributes[1] = VertexAttribute(sizeof(glm::vec2), VertexAttributeType::UV_COORDINATES);
	attributes[2] = VertexAttribute(sizeof(glm::vec3), VertexAttributeType::COLOR_0);
	attributes[3] = VertexAttribute(sizeof(glm::vec3), VertexAttributeType::NORMAL);

	auto asset = ImportAsset(pSceneState->pDevice, model, attributes);

	if (pSceneState->pGraphicsPipelines.find("Default") == pSceneState->pGraphicsPipelines.end())
		pSceneState->CreateDefaultPipeline();

	Flint::FBox2D windowExtent = pSceneState->pDisplay->GetExtent();
	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pDynamicStates->SetScissor(windowExtent, { 0, 0 });

	pShadowDynamicStates->SetViewPort(Flint::FExtent2D<float>{2048}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pShadowDynamicStates->SetScissor(Flint::FBox2D(2048), { 0, 0 });

	PrepareShadowMapPipeline();
	std::shared_ptr<Flint::GraphicsPipeline> pPipeline = pSceneState->pGraphicsPipelines["ShadowMap"];

	Flint::ImageSamplerSpecification samplerSpecification = {};
	pTextureSampler = pSceneState->pDevice->CreateImageSampler(samplerSpecification);

	samplerSpecification.mAddressModeU = Flint::AddressMode::CLAMP_TO_EDGE;
	samplerSpecification.mAddressModeV = Flint::AddressMode::CLAMP_TO_EDGE;
	samplerSpecification.mAddressModeW = Flint::AddressMode::CLAMP_TO_EDGE;
	samplerSpecification.mBorderColor = Flint::BorderColor::FLOAT_OPAQUE_WHITE;
	pShadowSampler = pSceneState->pDevice->CreateImageSampler(samplerSpecification);

	auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["ShadowMap"]->AddGeometry(asset.pVertexBuffer, asset.pIndexBuffer);
	UI32 imageIndex = 0;
	for (auto instance : asset.mDrawInstances)
	{
		if (instance.mName == "Plane")
			continue;

		//auto image = LoadImage(texture[imageIndex++]);
		//auto pTexture = pSceneState->pDevice->CreateImage(Flint::ImageType::DIMENSIONS_2, Flint::ImageUsage::GRAPHICS, image.mExtent, Flint::PixelFormat::R8G8B8A8_SRGB, 1, 1, image.pImageData);
		//pTextures.push_back(pTexture);
		//DestroyImage(image);

		pModelMatrixes.push_back(pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(glm::mat4)));
		std::shared_ptr<Flint::Buffer> pModelMatrix = pModelMatrixes.back();
		//SubmitToUniformBuffer(pModelMatrix, instance.mTransform);
		SubmitToUniformBuffer(pModelMatrix, glm::mat4(1.0f));
		//SubmitToUniformBuffer(pModelMatrix, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)));

		auto pResourceMap = pPipeline->CreateResourceMap();
		pResourceMap->SetResource("ubo", pModelMatrix);
		pResourceMap->SetResource("cam", pSceneState->mCamera.GetCameraBuffer());
		pResourceMap->SetResource("light", pLightUniform);
		pResourceMap->SetResource("shadowMap", pShadowSampler, pSceneState->pOffScreenRenderTargets["ShadowMap"]->GetResult(0));
		//pResourceMap->SetResource("texSampler", pTextureSampler, pTexture);

		mDrawIDs.push_back(pPipeline->AddDrawData(pResourceMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount));
		mVertexCount += instance.mVertexCount;
		mIndexCount += instance.mIndexCount;

		auto shadowResoutceMap = pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"]->CreateResourceMap();
		shadowResoutceMap->SetResource("light", pShadowMapUniform);
		shadowResoutceMap->SetResource("ubo", pModelMatrix);
		//shadowResoutceMap->SetResource("texSampler", pTextureSampler, pTexture);
		pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"]->AddDrawData(shadowResoutceMap, pShadowDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount);
	}

	pSceneState->pDevice->DestroyBuffer(asset.pVertexBuffer);
	pSceneState->pDevice->DestroyBuffer(asset.pIndexBuffer);

	//// Load the cube.
	//asset = ImportAsset(pSceneState->pDevice, pSceneState->GetAssetPath().string() + "\\Models\\Cube\\Cube.obj", attributes);
	//for (auto instance : asset.mDrawInstances)
	//{
	//	pLightObject = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(glm::mat4));
	//
	//	auto pResourceMap = pPipeline->CreateResourceMap();
	//	pResourceMap->SetResource("ubo", pLightObject);
	//	pResourceMap->SetResource("cam", pSceneState->mCamera.GetCameraBuffer());
	//	pResourceMap->SetResource("light", pLightUniform);
	//	pResourceMap->SetResource("shadowMap", pTextureSampler, pSceneState->pOffScreenRenderTargets["ShadowMap"]->GetResult(0));
	//
	//	mDrawIDs.push_back(pPipeline->AddDrawData(pResourceMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount));
	//	mVertexCount += instance.mVertexCount;
	//	mIndexCount += instance.mIndexCount;
	//
	//	auto shadowResoutceMap = pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"]->CreateResourceMap();
	//	shadowResoutceMap->SetResource("light", pShadowMapUniform);
	//	pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"]->AddDrawData(shadowResoutceMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount);
	//}

	mVertexOffset = vertexOffset;
	mIndexOffset = indexOffset;

	//pSceneState->pDevice->DestroyBuffer(asset.pVertexBuffer);
	//pSceneState->pDevice->DestroyBuffer(asset.pIndexBuffer);
}

Preview::~Preview()
{
	//pSceneState->pGeometryStores["Default"]->RemoveGeometry(mVertexOffset, mVertexCount, mIndexOffset, mIndexCount);

	for (const UI64 drawID : mDrawIDs)
		pSceneState->pGraphicsPipelines["Default"]->RemoveDrawData(drawID);
	mDrawIDs.clear();

	pSceneState->pDevice->DestroyImageSampler(pShadowSampler);
	pSceneState->pDevice->DestroyImageSampler(pTextureSampler);

	if (pTexture)
	{
		pSceneState->pDevice->DestroyImage(pTexture);
	}
	else
	{
		pSceneState->pDevice->DestroyShader(pVertexShader);
		pSceneState->pDevice->DestroyShader(pFragmentShader);

		pSceneState->pDevice->DestroyShader(pShadowVertexShader);
		pSceneState->pDevice->DestroyShader(pShadowFragmentShader);
	}

	for (auto pModelMatrix : pModelMatrixes)
		pSceneState->pDevice->DestroyBuffer(pModelMatrix);

	for (auto pTexture : pTextures)
		pSceneState->pDevice->DestroyImage(pTexture);

	pSceneState->pDevice->DestroyBuffer(pLightUniform);
	pSceneState->pDevice->DestroyBuffer(pShadowMapUniform);
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
			mLight.mLightPosition += mRotationBias * glm::vec3(1.0f, 0.0f, 0.0f);
		else
			mLight.mLightPosition += -mRotationBias * glm::vec3(1.0f, 0.0f, 0.0f);
	}

	// Rotate y
	if (pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).IsPressed() || pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).IsOnRepeat())
	{
		if ((pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).GetSpecialCharacter() & Flint::SpecialCharacter::SHIFT) == Flint::SpecialCharacter::SHIFT)
			mLight.mLightPosition += mRotationBias * glm::vec3(0.0f, 1.0f, 0.0f);
		else
			mLight.mLightPosition += -mRotationBias * glm::vec3(0.0f, 1.0f, 0.0f);
	}

	// Rotate z
	if (pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).IsPressed() || pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).IsOnRepeat())
	{
		if ((pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).GetSpecialCharacter() & Flint::SpecialCharacter::SHIFT) == Flint::SpecialCharacter::SHIFT)
			mLight.mLightPosition += mRotationBias * glm::vec3(0.0f, 0.0f, 1.0f);
		else
			mLight.mLightPosition += -mRotationBias * glm::vec3(0.0f, 0.0f, 1.0f);
	}

	glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 96.0f);
	glm::mat4 depthViewMatrix = glm::lookAt(mLight.mLightPosition, glm::vec3(0.0f), glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = glm::mat4(1.0f);

	//mLight.mLightPosition = pSceneState->mCamera.GetPosition();
	//mLight.mLightSpace = pSceneState->mCamera.GetMatrix().mProjectionMatrix * pSceneState->mCamera.GetMatrix().mViewMatrix * glm::mat4(1.0f);
	mLight.mLightSpace = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

	// Submit data to uniforms.
	SubmitToUniformBuffer(pShadowMapUniform, mLight.mLightSpace);
	SubmitToUniformBuffer(pLightUniform, mLight);
	SubmitToUniformBuffer(pModelUniform, mModelMatrix);
	//SubmitToUniformBuffer(pLightObject, glm::translate(glm::mat4(1.0f), mLight.mLightPosition - glm::vec3(1.0f)));
}

void Preview::PrepareNoTexturePipeline()
{
	if (pSceneState->pGraphicsPipelines["NoTexture"])
		return;

	pVertexShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\NoTexture\\shader.vert.spv"));
	pFragmentShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\NoTexture\\shader.frag.spv"));

	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pSceneState->pDevice->GetSupportedRasterizationSamples();
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::VIEWPORT | Flint::DynamicStateFlags::SCISSOR;

	pSceneState->pGraphicsPipelines["NoTexture"] = pSceneState->pDevice->CreateGraphicsPipeline("NoTexture", pSceneState->pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitPipeline(pSceneState->pGeometryStores["Default"], pSceneState->pGraphicsPipelines["NoTexture"]);
}

void Preview::PrepareShadowMapPipeline()
{
	if (pSceneState->pGraphicsPipelines["ShadowMap"])
		return;

	pSceneState->pOffScreenRenderTargets["ShadowMap"] = pSceneState->pDevice->CreateOffScreenRenderTarget(Flint::FBox2D(2048), pSceneState->pScreenBoundRenderTargets["Default"]->GetBufferCount(), { Flint::OffScreenResultSpecification(Flint::OffScreenRenderTargetAttachment::DEPTH_BUFFER, 1, Flint::PixelFormat::D32_SFLOAT) });
	pSceneState->pScreenBoundRenderTargets["Default"]->AttachOffScreenRenderTarget(pSceneState->pOffScreenRenderTargets["ShadowMap"]);

	pVertexShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\ShadowMapping\\mesh.vert.spv"));
	pFragmentShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\ShadowMapping\\mesh.frag.spv"));

	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pSceneState->pDevice->GetSupportedRasterizationSamples();
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::VIEWPORT | Flint::DynamicStateFlags::SCISSOR;

	pSceneState->pGeometryStores["ShadowMap"] = pSceneState->pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI32));

	pSceneState->pGraphicsPipelines["ShadowMap"] = pSceneState->pDevice->CreateGraphicsPipeline("ShadowMap", pSceneState->pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitPipeline(pSceneState->pGeometryStores["ShadowMap"], pSceneState->pGraphicsPipelines["ShadowMap"]);

	if (!pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"])
	{
		pShadowVertexShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\ShadowMapping\\shader.vert.spv"));
		pShadowFragmentShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\ShadowMapping\\shader.frag.spv"));

		specification.mRasterizationSamples = Flint::RasterizationSamples::BITS_1;
		specification.mColorBlendAttachments.clear();
		specification.bEnableDepthBias = true;
		specification.bEnableSampleShading = false;
		specification.mMinSampleShading = 0.0f;
		specification.mDepthSlopeFactor = 1.75f;
		specification.mDepthConstantFactor = 1.25f;
		//specification.mCullMode = Flint::CullMode::FRONT;

		pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"] = pSceneState->pDevice->CreateGraphicsPipeline("DefaultOffScreenWireFrame", pSceneState->pOffScreenRenderTargets["ShadowMap"], pShadowVertexShader, nullptr, nullptr, nullptr, pShadowFragmentShader, specification);
		pSceneState->pOffScreenRenderTargets["ShadowMap"]->SubmitPipeline(pSceneState->pGeometryStores["ShadowMap"], pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"]);
	}
}
