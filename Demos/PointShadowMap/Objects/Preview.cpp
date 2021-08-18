// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Preview.hpp"

#include "Flint/OffScreenRenderTargetFactory.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>

std::vector<glm::mat4> views;

Preview::Preview(glm::vec3 position, SceneState* pSceneState, std::filesystem::path model, std::vector<std::filesystem::path> texture)
	: GameObject(position, pSceneState)
{
	pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();
	pShadowDynamicStates = std::make_shared<Flint::DynamicStateContainer>();

	pSceneState->mCamera.SetCameraRange(0.1f, 1024.0f);

	mModelMatrix = glm::rotate(mModelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//SubmitToUniformBuffer(pModelMatrix, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	pLightUniform = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(glm::vec3));
	pShadowMapUniform = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(Light));
	pShadowMapCamera = pSceneState->pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(Camera));

	std::vector<VertexAttribute> attributes(4);
	attributes[0] = VertexAttribute(sizeof(glm::vec3), VertexAttributeType::POSITION);
	attributes[1] = VertexAttribute(sizeof(glm::vec2), VertexAttributeType::UV_COORDINATES);
	attributes[2] = VertexAttribute(sizeof(glm::vec3), VertexAttributeType::COLOR_0);
	attributes[3] = VertexAttribute(sizeof(glm::vec3), VertexAttributeType::NORMAL);

	auto asset = ImportAsset(pSceneState->pDevice, model, attributes);
	views.resize(asset.mDrawInstances.size() * 6);

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

	samplerSpecification.mAddressModeU = Flint::AddressMode::CLAMP_TO_BORDER;
	samplerSpecification.mAddressModeV = Flint::AddressMode::CLAMP_TO_BORDER;
	samplerSpecification.mAddressModeW = Flint::AddressMode::CLAMP_TO_BORDER;
	samplerSpecification.mBorderColor = Flint::BorderColor::FLOAT_OPAQUE_WHITE;
	pShadowSampler = pSceneState->pDevice->CreateImageSampler(samplerSpecification);

	auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["ShadowMap"]->AddGeometry(asset.pVertexBuffer, asset.pIndexBuffer);
	UI32 imageIndex = 0;
	for (UI32 i = 0; i < asset.mDrawInstances.size(); i++)
	{
		auto instance = asset.mDrawInstances[i];
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
		//SubmitToUniformBuffer(pModelMatrix, mModelMatrix);

		auto pResourceMap = pPipeline->CreateResourceMap();
		pResourceMap->SetResource("ubo", pModelMatrix);
		pResourceMap->SetResource("cam", pSceneState->mCamera.GetCameraBuffer());
		pResourceMap->SetResource("light", pLightUniform);
		pResourceMap->SetResource("shadowCubeMap", pShadowSampler, pSceneState->pOffScreenRenderTargets["ShadowMap"]->GetResult(0));
		//pResourceMap->SetResource("texSampler", pTextureSampler, pTexture);

		mDrawIDs.push_back(pPipeline->AddDrawData(pResourceMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount));
		mVertexCount += instance.mVertexCount;
		mIndexCount += instance.mIndexCount;

		auto shadowResoutceMap = pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"]->CreateResourceMap();
		shadowResoutceMap->SetResource("cam", pShadowMapCamera);
		shadowResoutceMap->SetResource("ubo", pShadowMapUniform);
		//shadowResoutceMap->SetResource("texSampler", pTextureSampler, pTexture);

		for (UI32 faceIndex = 0; faceIndex < 6; faceIndex++)
		{
			glm::mat4 viewMatrix = glm::mat4(1.0f);
			switch (faceIndex)
			{
			case 0: // POSITIVE_X
				viewMatrix = glm::rotate(viewMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case 1:	// NEGATIVE_X
				viewMatrix = glm::rotate(viewMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case 2:	// POSITIVE_Y
				viewMatrix = glm::rotate(viewMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case 3:	// NEGATIVE_Y
				viewMatrix = glm::rotate(viewMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case 4:	// POSITIVE_Z
				viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case 5:	// NEGATIVE_Z
				viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			}

			views[(i * 6) + faceIndex] = viewMatrix;

			auto pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();
			pDynamicStates->SetViewPort(Flint::FExtent2D<float>{1024}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
			pDynamicStates->SetScissor(Flint::FBox2D(1024), { 0, 0 });
			pDynamicStates->SetConstantData(Flint::ShaderType::VERTEX, &views[(i * 6) + faceIndex], sizeof(viewMatrix));

			pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"]->AddDrawData(shadowResoutceMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount);
		}
	}

	pSceneState->pDevice->DestroyBuffer(asset.pVertexBuffer);
	pSceneState->pDevice->DestroyBuffer(asset.pIndexBuffer);

	mVertexOffset = vertexOffset;
	mIndexOffset = indexOffset;
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
	pSceneState->pDevice->DestroyBuffer(pShadowMapCamera);
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


#define SCALE		glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f))
#define ROTATE		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
#define TRANSLATE	glm::translate(glm::mat4(1.0f), glm::vec3(-mLight.mLightPosition.x, -mLight.mLightPosition.y, -mLight.mLightPosition.z))

	mCamera.mProjection = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 1024.0f);
	//mLight.mModel = mModelMatrix * TRANSLATE;
	mLight.mModel = TRANSLATE;
	//mLight.mModel = glm::mat4(1.0f);

	//mLight.mLightPosition = glm::vec3(mModelMatrix * glm::vec4(mLight.mLightPosition, 1.0f));

	// Submit data to uniforms.
	SubmitToUniformBuffer(pShadowMapUniform, mLight);
	SubmitToUniformBuffer(pShadowMapCamera, mCamera);
	SubmitToUniformBuffer(pLightUniform, mLight.mLightPosition);
	//SubmitToUniformBuffer(pModelUniform, mModelMatrix);
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

	auto pFactory = pSceneState->pDevice->CreateOffScreenRenderTargetFactory();

	pSceneState->pOffScreenRenderTargets["ShadowMap"] = pFactory->Create(Flint::OffScreenRenderTargetType::POINT_SHADOW_MAP, Flint::FBox2D(1024), pSceneState->pScreenBoundRenderTargets["Default"]->GetBufferCount());;
	pSceneState->pScreenBoundRenderTargets["Default"]->AttachOffScreenRenderTarget(pSceneState->pOffScreenRenderTargets["ShadowMap"]);

	pVertexShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\PointShadowMapping\\mesh.vert.spv"));
	pFragmentShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\PointShadowMapping\\mesh.frag.spv"));

	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pSceneState->pDevice->GetSupportedRasterizationSamples();
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::VIEWPORT | Flint::DynamicStateFlags::SCISSOR;

	pSceneState->pGeometryStores["ShadowMap"] = pSceneState->pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI32));

	pSceneState->pGraphicsPipelines["ShadowMap"] = pSceneState->pDevice->CreateGraphicsPipeline("ShadowMap", pSceneState->pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitPipeline(pSceneState->pGeometryStores["ShadowMap"], pSceneState->pGraphicsPipelines["ShadowMap"]);

	if (!pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"])
	{
		pShadowVertexShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\PointShadowMapping\\shader.vert.spv"));
		pShadowFragmentShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\PointShadowMapping\\shader.frag.spv"));

		specification.mRasterizationSamples = Flint::RasterizationSamples::BITS_1;
		//specification.mColorBlendAttachments.clear();
		specification.bEnableDepthBias = true;
		specification.bEnableDepthTest = true;
		specification.bEnableSampleShading = false;
		specification.mMinSampleShading = 0.0f;
		//specification.mDepthSlopeFactor = 1.75f;
		//specification.mDepthConstantFactor = 1.25f;
		//specification.mCullMode = Flint::CullMode::FRONT;

		pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"] = pSceneState->pDevice->CreateGraphicsPipeline("DefaultOffScreenWireFrame", pSceneState->pOffScreenRenderTargets["ShadowMap"], pShadowVertexShader, nullptr, nullptr, nullptr, pShadowFragmentShader, specification);
		pSceneState->pOffScreenRenderTargets["ShadowMap"]->SubmitPipeline(pSceneState->pGeometryStores["ShadowMap"], pSceneState->pGraphicsPipelines["DefaultOffScreenWireFrame"]);
	}
}
