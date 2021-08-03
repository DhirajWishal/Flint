// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SceneState.hpp"

#include "Flint/Instance.hpp"

SceneState::SceneState()
{
#ifndef FLINT_RELEASE
	pInstance = Flint::CreateInstance(true);

#else
	pInstance = Flint::CreateInstance(false);

#endif // !FLINT_RELEASE

	//pDisplay = pInstance->CreateDisplay(Flint::FBox2D(), "Flint: Sample Scene");
	pDisplay = pInstance->CreateDisplay(Flint::FBox2D(1280, 720), "Flint: Sample Scene");
	pDevice = pInstance->CreateDevice(Flint::DeviceFlags::GRAPHICS_COMPATIBLE | Flint::DeviceFlags::EXTERNAL | Flint::DeviceFlags::COMPUTE_COMPATIBLE);

	pVertexShader = pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\3D\\shader.vert.spv"), Flint::ShaderCodeType::SPIR_V);
	pFragmentShader = pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\3D\\shader.frag.spv"), Flint::ShaderCodeType::SPIR_V);
	pGeometryStores["Default"] = pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI32));

	CreateDefaultRenderTarget();

	mCamera.SetAspectRatio(pDisplay->GetExtent());
}

SceneState::~SceneState()
{
	for (auto pRenderTarget : pScreenBoundRenderTargets)
		pDevice->DestroyRenderTarget(pRenderTarget.second);

	for (auto pPipeline : pGraphicsPipelines)
		pDevice->DestroyPipeline(pPipeline.second);

	for (auto pGeometryStore : pGeometryStores)
		pDevice->DestroyGeometryStore(pGeometryStore.second);

	pDevice->DestroyShader(pVertexShader);
	pDevice->DestroyShader(pFragmentShader);

	pInstance->DestroyDisplay(pDisplay);
	pInstance->DestroyDevice(pDevice);
	pInstance->Terminate();
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

void SceneState::UpdateCamera()
{
	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_W).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_W).IsOnRepeat())
		mCamera.WalkUp();

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_A).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_A).IsOnRepeat())
		mCamera.WalkLeft();

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_S).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_S).IsOnRepeat())
		mCamera.WalkDown();

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_D).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_D).IsOnRepeat())
		mCamera.WalkRight();

	if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsPressed())
		mCamera.MousePosition(pDisplay->GetMousePosition());

	if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsReleased())
		mCamera.ResetFirstMouse();

	mCamera.Update();
}

void SceneState::CreateDefaultRenderTarget()
{
	pScreenBoundRenderTargets["Default"] = pDevice->CreateScreenBoundRenderTarget(pDisplay, pDisplay->GetExtent(), pDisplay->FindBestBufferCount(pDevice), 1);
}

void SceneState::CreateDefaultPipeline()
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

	pGraphicsPipelines["Default"] = pDevice->CreateGraphicsPipeline("Default", pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pScreenBoundRenderTargets["Default"]->SubmitPipeline(pGeometryStores["Default"], pGraphicsPipelines["Default"]);
}
