// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "EditorRenderTarget.hpp"

#include "GraphicsCore/Instance.hpp"

namespace Flint
{
	void EditorRenderTarget::Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Instance>& pInstnace)
	{
		const FBox2D extent = FBox2D(1280, 720);
		pDisplay = pInstnace->CreateDisplay(extent, "Flint Editor");

		const auto sampleCount = pDevice->GetSupportedMultiSampleCount();
		const FBox3D imageExtent = FBox3D(extent.mWidth, extent.mHeight, 1);

		// rgb(171, 221, 252)

		mAttachments[0] = RenderTargetAttachment(
			pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color, imageExtent, pDisplay->GetBestSwapChainFormat(pDevice), 1, 1, nullptr, sampleCount),
			FColor4D(CreateColor256(171.0f), CreateColor256(221.0f), CreateColor256(252.0f), 1.0f));

		mAttachments[1] = RenderTargetAttachment(
			pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Depth, imageExtent, PixelFormat::D24_UNORMAL_S8_UINT, 1, 1, nullptr, sampleCount),
			DepthClearValues(1.0f, 0));

		pRenderTarget = pDevice->CreateScreenBoundRenderTarget(pDisplay, extent, pDisplay->FindBestBufferCount(pDevice), mAttachments, SwapChainPresentMode::MailBox);

		pAllocator = pDevice->CreateCommandBufferAllocator(pRenderTarget->GetBufferCount());
		pSecondaryAllocator = pAllocator->CreateChildAllocator();

		pAllocator->CreateCommandBuffers();
		pSecondaryAllocator->CreateCommandBuffers();

		mImGuiAdapter.Initialize(pDevice, pRenderTarget);

		mVikingRoom.Initialize(pDevice, pRenderTarget, &mCamera);
		mCamera.SetAspectRatio(pDisplay->GetExtent());
		//mSceneRenderTarget.Initialize(pDevice, pDisplay->FindBestBufferCount(pDevice), )
	}

	void EditorRenderTarget::Terminate()
	{
		//mSceneRenderTarget.Terminate();
		pRenderTarget->Terminate();

		pAllocator->Terminate();
		pSecondaryAllocator->Terminate();
		pDisplay->Terminate();

		mImGuiAdapter.Terminate();
		mVikingRoom.Terminate();

		for (auto& attachment : mAttachments)
			attachment.pImage->Terminate();
	}

	bool EditorRenderTarget::IsDisplayOpen() const
	{
		return pDisplay->IsOpen();
	}

	void EditorRenderTarget::PollEvents(UI64 delta)
	{
		pDisplay->Update();

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = delta / 1000.0f;

		auto extent = pDisplay->GetExtent();
		if (!extent.IsZero())
		{
			io.DisplaySize = ImVec2(static_cast<float>(extent.mWidth), static_cast<float>(extent.mHeight));
			mCamera.SetAspectRatio(extent);
		}

		auto position = pDisplay->GetMousePosition();
		io.MousePos = ImVec2(position.X, position.Y);

		if (pDisplay->GetMouseButtonEvent(MouseButton::Left).IsPressed() || pDisplay->GetMouseButtonEvent(MouseButton::Left).IsOnRepeat())
			io.MouseDown[0] = true;
		else if (pDisplay->GetMouseButtonEvent(MouseButton::Left).IsReleased())
			io.MouseDown[0] = false;

		if (pDisplay->GetMouseButtonEvent(MouseButton::Right).IsPressed() || pDisplay->GetMouseButtonEvent(MouseButton::Right).IsOnRepeat())
			io.MouseDown[1] = true;
		else if (pDisplay->GetMouseButtonEvent(MouseButton::Right).IsReleased())
			io.MouseDown[1] = false;

		// Update the camera.
		if (pDisplay->GetKeyEvent(KeyCode::KeyW).IsPressed() || pDisplay->GetKeyEvent(KeyCode::KeyW).IsOnRepeat())
			mCamera.MoveFront(delta);

		if (pDisplay->GetKeyEvent(KeyCode::KeyA).IsPressed() || pDisplay->GetKeyEvent(KeyCode::KeyA).IsOnRepeat())
			mCamera.MoveLeft(delta);

		if (pDisplay->GetKeyEvent(KeyCode::KeyS).IsPressed() || pDisplay->GetKeyEvent(KeyCode::KeyS).IsOnRepeat())
			mCamera.MoveBack(delta);

		if (pDisplay->GetKeyEvent(KeyCode::KeyD).IsPressed() || pDisplay->GetKeyEvent(KeyCode::KeyD).IsOnRepeat())
			mCamera.MoveRight(delta);

		if (pDisplay->GetMouseButtonEvent(MouseButton::Left).IsPressed() && !io.WantCaptureMouse)
			mCamera.MousePosition(pDisplay->GetMousePosition());

		if (pDisplay->GetMouseButtonEvent(MouseButton::Left).IsReleased())
			mCamera.ResetFirstMouse();

		mCamera.Update(delta);
	}

	void EditorRenderTarget::DrawFrame()
	{
		if (!pRenderTarget->PrepareNewFrame())
		{
			pRenderTarget->Recreate();
			mVikingRoom.Recreate();
			return;
		}

		auto pCommandBuffer = pAllocator->GetCommandBuffer(pRenderTarget->GetImageIndex());
		auto pSecondaryCommandBuffer = pSecondaryAllocator->GetCommandBuffer(pRenderTarget->GetImageIndex());

		pCommandBuffer->BeginBufferRecording();
		pCommandBuffer->BindRenderTargetSecondary(pRenderTarget);

		pSecondaryCommandBuffer->BeginBufferRecording(pRenderTarget);

		mVikingRoom.SubmitToCommandBuffer(pSecondaryCommandBuffer);
		mImGuiAdapter.Render(pSecondaryCommandBuffer);

		pSecondaryCommandBuffer->EndBufferRecording();

		pCommandBuffer->SubmitSecondaryCommandBuffer(pSecondaryCommandBuffer);
		pCommandBuffer->ExecuteSecondaryCommands();

		pCommandBuffer->UnbindRenderTarget();
		pCommandBuffer->EndBufferRecording();

		pRenderTarget->GetDevice()->SubmitGraphicsCommandBuffers({ pCommandBuffer });

		if (!pRenderTarget->PresentToDisplay())
		{
			pRenderTarget->Recreate();
			mVikingRoom.Recreate();
		}

		pRenderTarget->IncrementFrameIndex();
		pRenderTarget->GetDevice()->WaitForQueue();
	}

	void EditorRenderTarget::UpdateUI(const UI64 delta)
	{
	}
}
