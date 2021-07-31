// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SceneState.hpp"

#include "Backend/Instance.hpp"

SceneState::SceneState()
{
#ifndef FLINT_RELEASE
	pInstance = Flint::CreateInstance(true);

#else
	pInstance = Flint::CreateInstance(false);

#endif // !FLINT_RELEASE

	pDisplay = pInstance->CreateDisplay(mDisplayExtent, "Flint: Sample Scene");
	pDevice = pInstance->CreateDevice(Flint::DeviceFlags::GRAPHICS_COMPATIBLE | Flint::DeviceFlags::EXTERNAL | Flint::DeviceFlags::COMPUTE_COMPATIBLE);

	CreateDefaultRenderTarget();
	CreateDefaultPipeline();

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

	pInstance->DestroyDisplay(pDisplay);
	pInstance->DestroyDevice(pDevice);
	pInstance->Terminate();
}

void SceneState::PrepareRenderTargetsToDraw()
{
	for (auto pRenderTarget : pScreenBoundRenderTargets)
		pRenderTarget.second->PrepareStaticResources();
}

void SceneState::PrepareNewFrame()
{
	for (auto pRenderTarget : pScreenBoundRenderTargets)
		pRenderTarget.second->BeginFrame();

	if (pDisplay->IsDisplayResized())
	{
		auto extent = pDisplay->GetExtent();
		if (extent.mWidth > 0 && extent.mHeight > 0)
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
	pScreenBoundRenderTargets["Default"] = pDevice->CreateScreenBoundRenderTarget(pDisplay, mDisplayExtent, pDisplay->FindBestBufferCount(pDevice));
}

void SceneState::CreateDefaultPipeline()
{
}
