// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "CameraController.hpp"
#include "Engine/ClientInterface.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"

CameraController::CameraController(Flint::ClientInterface* pClientInterface)
	: Controller("Camera Controller", pClientInterface)
{
	mCameraHandle = pClientInterface->CreateGlobalComponent<Flint::Components::Camera>("DefaultCamera", "Default Camera");
}

void CameraController::OnUpdate(const UI64 delta, const Flint::UpdateSpecification specification)
{
	auto pDisplay = pClientInterface->GetDefaultScreenBoundRenderTarget()->GetDisplay();
	auto pCamera = mCameraHandle.GetComponent();
	pCamera->mAspectRatio = static_cast<float>(pDisplay->GetExtent().mWidth) / static_cast<float>(pDisplay->GetExtent().mHeight);

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyW).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyW).IsOnRepeat())
		pCamera->mCameraVectors.mPosition += pCamera->mCameraVectors.mFront * (static_cast<float>(delta) / 100000000) * mMovementBias;

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyS).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyS).IsOnRepeat())
		pCamera->mCameraVectors.mPosition -= pCamera->mCameraVectors.mFront * (static_cast<float>(delta) / 100000000) * mMovementBias;

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyD).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyD).IsOnRepeat())
		pCamera->mCameraVectors.mPosition += pCamera->mCameraVectors.mRight * (static_cast<float>(delta) / 100000000) * mMovementBias;

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyA).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyA).IsOnRepeat())
		pCamera->mCameraVectors.mPosition -= pCamera->mCameraVectors.mRight * (static_cast<float>(delta) / 100000000) * mMovementBias;

	if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::Left).IsPressed() && !specification.bSkipMousePosition)
		ProcessMouse(pDisplay->GetMousePosition());
	
	if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::Left).IsReleased())
		bFirstMouse = true;

	pCamera->Update();
}

void CameraController::ProcessMouse(Flint::FExtent2D<float> position)
{
	auto pCamera = mCameraHandle.GetComponent();
	position.X *= -1.0f;
	position.Y *= -1.0f;

	if (bFirstMouse)
	{
		mLastX = position.X;
		mLastY = position.Y;
		bFirstMouse = false;
	}

	float xoffset = position.X - mLastX;
	float yoffset = mLastY - position.Y; // reversed since y-coordinates go from bottom to top

	float sensitivity = 0.05f;
	xoffset *= sensitivity * 0.75f;
	yoffset *= sensitivity;

	mLastX = position.X;
	mLastY = position.Y;

	pCamera->mYaw += xoffset;
	pCamera->mPitch += yoffset;

	if (pCamera->mPitch > 89.0f)
		pCamera->mPitch = 89.0f;
	if (pCamera->mPitch < -89.0f)
		pCamera->mPitch = -89.0f;
}
