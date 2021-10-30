// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "BasicController.hpp"
#include "Engine/ClientInterface.hpp"
#include "GraphicsCore/ResourcePackager.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "Engine/Components/Camera.hpp"

BasicController::BasicController(Flint::ClientInterface* pClientInterface)
	: Flint::Controller("BasicController", pClientInterface), pDynamicStates(std::make_shared<Flint::DynamicStateContainer>())
{
	pClientInterface->ActivateController("CameraController");

	SetupMaterialPipeline();
	SetupStaticModel();
	SetupDrawInstance();

	mUniformMVP = pClientInterface->CreateComponent<Flint::Components::ModelViewProjectionUniform>("Main Camera", pClientInterface->GetDefaultDevice());
	pPackage->BindResource(0, mUniformMVP->pUniformBuffer);
}

void BasicController::OnUpdate(const uint64 delta, const Flint::UpdateSpecification specification)
{
	Flint::Components::Camera* pCamera = pClientInterface->GetComponent<Flint::Components::Camera>("DefaultCamera");
	auto pUniform = mUniformMVP.GetComponent();

	pUniform->mMatrix.mProjection = pCamera->mViewProjection.mProjectionMatrix;
	pUniform->mMatrix.mView = pCamera->mViewProjection.mViewMatrix;
	pUniform->UpdateUniform();
}

void BasicController::SetupMaterialPipeline()
{
	mMaterialHandle = pClientInterface->CreateComponent<Flint::Components::MaterialPipeline>("Basic component");

	auto pRenderTarget = pClientInterface->GetDefaultScreenBoundRenderTarget();
	auto pMaterialPipeline = mMaterialHandle.GetComponent();
	pMaterialPipeline->pVertexShader = pClientInterface->CompileAndCreateShader("E:\\Flint\\Demos\\SampleScene\\Shaders\\Debug\\Shader.vert", Flint::ShaderCodeType::GLSL, Flint::ShaderType::Vertex);
	pMaterialPipeline->pFragmentShader = pClientInterface->CompileAndCreateShader("E:\\Flint\\Demos\\SampleScene\\Shaders\\Debug\\Shader.frag", Flint::ShaderCodeType::GLSL, Flint::ShaderType::Fragment);

	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pClientInterface->GetDefaultDevice()->GetSupportedMultiSampleCount();
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::ViewPort | Flint::DynamicStateFlags::Scissor;
	specification.bEnableDepthTest = true;
	specification.bEnableDepthWrite = true;
	specification.mColorBlendConstants[0] = 0.0f;
	specification.mColorBlendConstants[1] = 0.0f;
	specification.mColorBlendConstants[2] = 0.0f;
	specification.mColorBlendConstants[3] = 0.0f;
	pMaterialPipeline->pPipeline = pClientInterface->CreateGraphicsPipeline("Default", pRenderTarget, pMaterialPipeline->pVertexShader, pMaterialPipeline->pFragmentShader, specification);

	auto packagers = pMaterialPipeline->pPipeline->CreateResourcePackagers();
	pPackager = packagers.front();
	pPackage = pPackager->CreatePackage();
}

void BasicController::SetupStaticModel()
{
	mStaticModelHandle = pClientInterface->CreateComponent<Flint::Components::StaticModel>("Basic static model");
	pClientInterface->LoadStaticGeometry(mStaticModelHandle, "E:\\Flint\\Assets\\Packages\\Tree001\\Tree01\\Tree1\\Tree1.obj", mMaterialHandle->pVertexShader, Flint::Defaults::CreateDefaultVertexDescriptor());
}

void BasicController::SetupDrawInstance()
{
	mDrawInstance = pClientInterface->CreateComponent<Flint::Components::DrawInstanceGraphics>("Model Draw", mStaticModelHandle->pGeometryStore, mMaterialHandle->pPipeline);
	auto pDrawInstance = mDrawInstance.GetComponent();

	for (auto& wireFrame : mStaticModelHandle->mWireFrames)
		pDrawInstance->AddDrawData(&wireFrame, { pPackage }, pDynamicStates);

	const auto windowExtent = pClientInterface->GetDefaultScreenBoundRenderTarget()->GetExtent();
	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pDynamicStates->SetScissor(windowExtent, { 0, 0 });
}
