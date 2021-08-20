// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SceneState.hpp"
#include "ShaderCompiler.hpp"

#include "Flint/Instance.hpp"

SceneState::SceneState(const std::string& displayTitle)
{
	mSolutionPath = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path().parent_path();
	mAssetPath = mSolutionPath.string() + "\\Assets";

	mCamera.SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));

#ifndef FLINT_RELEASE
	//pInstance = Flint::CreateInstance(false);
	pInstance = Flint::CreateInstance(true);

#else
	pInstance = Flint::CreateInstance(false);

#endif // !FLINT_RELEASE

	CompileAllShaders(mSolutionPath);

	//pDisplay = pInstance->CreateDisplay(Flint::FBox2D(), "Flint: Sample Scene");
	pDisplay = pInstance->CreateDisplay(Flint::FBox2D(1280, 720), displayTitle);
	pDevice = pInstance->CreateDevice(Flint::DeviceFlags::GRAPHICS_COMPATIBLE | Flint::DeviceFlags::EXTERNAL | Flint::DeviceFlags::COMPUTE_COMPATIBLE);

	pVertexShader = pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path(mAssetPath.string() + "\\Shaders\\3D\\shader.vert.spv"));
	pFragmentShader = pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path(mAssetPath.string() + "\\Shaders\\3D\\shader.frag.spv"));
	pGeometryStores["Default"] = pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI32));

	CreateDefaultRenderTarget();
	CreateBoundingBoxPipeline();
	//CreateDefaultPipeline();

	mCamera.SetAspectRatio(pDisplay->GetExtent());

	mCamera.Initialize(pDevice);
}

SceneState::~SceneState()
{
	for (auto pRenderTarget : pScreenBoundRenderTargets)
		pRenderTarget.second->Terminate();

	for (auto pRenderTarget : pOffScreenRenderTargets)
		pRenderTarget.second->Terminate();

	for (auto pPipeline : pGraphicsPipelines)
		pPipeline.second->Terminate();

	for (auto pGeometryStore : pGeometryStores)
		pGeometryStore.second->Terminate();

	pVertexShader->Terminate();
	pFragmentShader->Terminate();

	pVertexShaderBB->Terminate();
	pFragmentShaderBB->Terminate();

	mCamera.Terminate();

	//pInstance->DestroyDisplay(pDisplay);
	//pInstance->DestroyDevice(pDevice);
	//pInstance->Terminate();
}

void SceneState::PrepareRenderTargetsToDraw()
{
	for (auto pRenderTarget : pScreenBoundRenderTargets)
	{
		//pRenderTarget.second->SetClearColor(Flint::FColor4D(CREATE_COLOR_256(135.0f), CREATE_COLOR_256(206.0f), CREATE_COLOR_256(235.0f), 1));
		pRenderTarget.second->PrepareStaticResources();
	}
}

void SceneState::PrepareNewFrame()
{
	for (auto pRenderTarget : pScreenBoundRenderTargets)
		pRenderTarget.second->BeginFrame();

	if (pDisplay->IsDisplayResized())
	{
		auto extent = pDisplay->GetExtent();
		if (!extent.IsZero())
			mCamera.SetAspectRatio(extent);
	}
}

void SceneState::SubmitFrames()
{
	for (auto pRenderTarget : pScreenBoundRenderTargets)
		pRenderTarget.second->SubmitFrame();
}

void SceneState::UpdateCamera(UI64 delta)
{
	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_W).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_W).IsOnRepeat())
		mCamera.WalkUp(delta);

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_A).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_A).IsOnRepeat())
		mCamera.WalkLeft(delta);

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_S).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_S).IsOnRepeat())
		mCamera.WalkDown(delta);

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_D).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_D).IsOnRepeat())
		mCamera.WalkRight(delta);

	if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsPressed())
		mCamera.MousePosition(pDisplay->GetMousePosition(), delta);

	if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsReleased())
		mCamera.ResetFirstMouse();

	mCamera.Update(delta);
}

void SceneState::CreateDefaultRenderTarget()
{
	pScreenBoundRenderTargets["Default"] = pDevice->CreateScreenBoundRenderTarget(pDisplay, pDisplay->GetExtent(), pDisplay->FindBestBufferCount(pDevice), 1);
}

void SceneState::CreateBoundingBoxPipeline()
{
	pVertexShaderBB = pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path(GetAssetPath().string() + "\\Shaders\\BoundingBox\\shader.vert.spv"));
	pFragmentShaderBB = pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path(GetAssetPath().string() + "\\Shaders\\BoundingBox\\shader.frag.spv"));

	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pDevice->GetSupportedRasterizationSamples();
	specification.mPrimitiveTopology = Flint::PrimitiveTopology::LINE_LIST;
	specification.mPolygonMode = Flint::PolygonMode::LINE;

	pGraphicsPipelines["BoundingBox"] = pDevice->CreateGraphicsPipeline("BoundingBox", pScreenBoundRenderTargets["Default"], pVertexShaderBB, nullptr, nullptr, nullptr, pFragmentShaderBB, specification);
}

void SceneState::CreateDefaultPipeline()
{
	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pDevice->GetSupportedRasterizationSamples();
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::VIEWPORT | Flint::DynamicStateFlags::SCISSOR;
	specification.bEnableDepthTest = true;
	specification.bEnableDepthWrite = true;
	specification.mColorBlendConstants[0] = 0.0f;
	specification.mColorBlendConstants[1] = 0.0f;
	specification.mColorBlendConstants[2] = 0.0f;
	specification.mColorBlendConstants[3] = 0.0f;
	//specification.mPrimitiveTopology = Flint::PrimitiveTopology::POINT_LIST;
	//specification.mPolygonMode = Flint::PolygonMode::LINE;

	pGraphicsPipelines["Default"] = pDevice->CreateGraphicsPipeline("Default", pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pScreenBoundRenderTargets["Default"]->SubmitGraphicsPipeline(pGeometryStores["Default"], pGraphicsPipelines["Default"]);
}
