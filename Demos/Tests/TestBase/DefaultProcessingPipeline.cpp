// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultProcessingPipeline.hpp"

#include <optick.h>
#include <imgui.h>

namespace Flint
{
	DefaultProcessingPipeline::DefaultProcessingPipeline(const std::shared_ptr<Device>& pDevice, const FBox2D frameExtent, const std::string& displayTitle, const UI32 pipelineCount, const MultiSampleCount msaaCount, const bool forceColorBuffer, const bool forceDepthBuffer)
		: ProcessingPipeline(pDevice, frameExtent, displayTitle, pipelineCount, msaaCount, forceColorBuffer, forceDepthBuffer)
	{
		mCamera.SetAspectRatio(pDisplay->GetExtent());
	}

	UI64 DefaultProcessingPipeline::Update()
	{
		OPTICK_EVENT();

		// Get the current time point and delta.
		const auto currentTimePoint = std::chrono::steady_clock::now();
		const UI64 delta = currentTimePoint.time_since_epoch().count() - mOldTimePoint.time_since_epoch().count();

		// Get the display and update it.
		const auto pDisplay = GetDisplay();
		pDisplay->Update();

		// Setup ImGui.
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = delta / 1000.0f;

		ImGui::Begin("Statistics");
		ImGui::Text("Frame time: %.4f ms (%d FPS)", io.DeltaTime / 1000.0f, static_cast<UI32>(1000.0f / (io.DeltaTime / 1000.0f)));
		ImGui::End();

		const auto extent = pDisplay->GetExtent();
		if (!extent.IsZero())
			io.DisplaySize = ImVec2(static_cast<float>(extent.mWidth), static_cast<float>(extent.mHeight));

		const auto position = pDisplay->GetMousePosition();
		io.MousePos = ImVec2(position.X, position.Y);

		// Process mouse inputs.
		if (pDisplay->GetMouseButtonEvent(MouseButton::Left).IsPressed() || pDisplay->GetMouseButtonEvent(MouseButton::Left).IsOnRepeat())
			io.MouseDown[0] = true;

		else if (pDisplay->GetMouseButtonEvent(MouseButton::Left).IsReleased())
			io.MouseDown[0] = false;

		if (pDisplay->GetMouseButtonEvent(MouseButton::Right).IsPressed() || pDisplay->GetMouseButtonEvent(MouseButton::Right).IsOnRepeat())
			io.MouseDown[1] = true;

		else if (pDisplay->GetMouseButtonEvent(MouseButton::Right).IsReleased())
			io.MouseDown[1] = false;

		// Process the camera movements.
		if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyW).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyW).IsOnRepeat())
			mCamera.MoveFront(delta);

		if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyA).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyA).IsOnRepeat())
			mCamera.MoveLeft(delta);

		if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyS).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyS).IsOnRepeat())
			mCamera.MoveBack(delta);

		if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyD).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KeyD).IsOnRepeat())
			mCamera.MoveRight(delta);

		if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::Left).IsPressed() && !io.WantCaptureMouse)
			mCamera.MousePosition(position);

		if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::Left).IsReleased())
			mCamera.ResetFirstMouse();

		// Update the camera.
		mCamera.Update();

		mOldTimePoint = currentTimePoint;
		return delta;
	}
}
