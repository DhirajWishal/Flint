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
	pDevice = pInstance->CreateDevice(Flint::DeviceFlags::GraphicsCompatible | Flint::DeviceFlags::External | Flint::DeviceFlags::ComputeCompatible);

	pVertexShader = pDevice->CreateShader(Flint::ShaderType::Vertex, std::filesystem::path("Flint\\Shaders\\3D\\shader.vert.spv"));
	pFragmentShader = pDevice->CreateShader(Flint::ShaderType::Fragment, std::filesystem::path("Flint\\Shaders\\3D\\shader.frag.spv"));
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
		//pRenderTarget.second->SetClearColor(Flint::FColor4D(CreateColor256(135.0f), CreateColor256(206.0f), CreateColor256(235.0f), 1));
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
	if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyW).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyW).IsOnRepeat())
		mCamera.WalkUp(delta);

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyA).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyA).IsOnRepeat())
		mCamera.WalkLeft(delta);

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyS).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyS).IsOnRepeat())
		mCamera.WalkDown(delta);

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyD).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyD).IsOnRepeat())
		mCamera.WalkRight(delta);

	if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::Left).IsPressed())
		mCamera.MousePosition(pDisplay->GetMousePosition(), delta);

	if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::Left).IsReleased())
		mCamera.ResetFirstMouse();

	mCamera.Update(delta);
}

void SceneState::CreateDefaultRenderTarget()
{
	pScreenBoundRenderTargets["Default"] = pDevice->CreateScreenBoundRenderTarget(pDisplay, pDisplay->GetExtent(), pDisplay->FindBestBufferCount(pDevice), 0);
}

void SceneState::CreateBoundingBoxPipeline()
{
	pVertexShaderBB = pDevice->CreateShader(Flint::ShaderType::Vertex, std::filesystem::path("Flint\\Shaders\\BoundingBox\\shader.vert.spv"));
	pFragmentShaderBB = pDevice->CreateShader(Flint::ShaderType::Fragment, std::filesystem::path("Flint\\Shaders\\BoundingBox\\shader.frag.spv"));

	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pDevice->GetSupportedMultiSampleCount();
	specification.mPrimitiveTopology = Flint::PrimitiveTopology::LineList;
	specification.mPolygonMode = Flint::PolygonMode::Line;

	pGraphicsPipelines["BoundingBox"] = pDevice->CreateGraphicsPipeline("BoundingBox", pScreenBoundRenderTargets["Default"], pVertexShaderBB, nullptr, nullptr, nullptr, pFragmentShaderBB, specification);
}

void SceneState::CreateDefaultPipeline()
{
	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pDevice->GetSupportedMultiSampleCount();
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::ViewPort | Flint::DynamicStateFlags::Scissor;
	specification.bEnableDepthTest = true;
	specification.bEnableDepthWrite = true;
	specification.mColorBlendConstants[0] = 0.0f;
	specification.mColorBlendConstants[1] = 0.0f;
	specification.mColorBlendConstants[2] = 0.0f;
	specification.mColorBlendConstants[3] = 0.0f;
	//specification.mPrimitiveTopology = Flint::PrimitiveTopology::PointList;
	//specification.mPolygonMode = Flint::PolygonMode::Line;

	pGraphicsPipelines["Default"] = pDevice->CreateGraphicsPipeline("Default", pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pScreenBoundRenderTargets["Default"]->SubmitGraphicsPipeline(pGeometryStores["Default"], pGraphicsPipelines["Default"]);
}