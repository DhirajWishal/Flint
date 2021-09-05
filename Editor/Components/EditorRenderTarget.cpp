// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "EditorRenderTarget.hpp"

#include "GraphicsCore/Instance.hpp"

namespace Flint
{
	void EditorRenderTarget::Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Instance>& pInstnace)
	{
		const FBox2D extent = FBox2D(1280, 720);
		const FBox2D offScreenExtent = FBox2D(848, 480);
		pDisplay = pInstnace->CreateDisplay(extent, "Flint Editor");

		const auto sampleCount = pDevice->GetSupportedMultiSampleCount();

		pRenderTarget = pDevice->CreateScreenBoundRenderTarget(pDisplay, extent, pDisplay->FindBestBufferCount(pDevice), {}, SwapChainPresentMode::MailBox);

		pAllocator = pDevice->CreateCommandBufferAllocator(pRenderTarget->GetBufferCount());
		pSecondaryAllocator = pAllocator->CreateChildAllocator();

		pAllocator->CreateCommandBuffers();
		pSecondaryAllocator->CreateCommandBuffers();

		mImGuiAdapter.Initialize(pDevice, pRenderTarget);
		mSceneRenderTarget.Initialize(pDevice, pRenderTarget->GetBufferCount(), offScreenExtent, mImGuiAdapter.CreateResourcePackage());
	}

	void EditorRenderTarget::Terminate()
	{
		mSceneRenderTarget.Terminate();
		pRenderTarget->Terminate();

		pAllocator->Terminate();
		pSecondaryAllocator->Terminate();
		pDisplay->Terminate();

		mImGuiAdapter.Terminate();
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
			io.DisplaySize = ImVec2(static_cast<float>(extent.mWidth), static_cast<float>(extent.mHeight));

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
	}

	void EditorRenderTarget::DrawFrame()
	{
		if (!pRenderTarget->PrepareNewFrame())
		{
			pRenderTarget->Recreate();
			return;
		}

		auto pCommandBuffer = pAllocator->GetCommandBuffer(pRenderTarget->GetImageIndex());
		auto pSecondaryCommandBuffer = pSecondaryAllocator->GetCommandBuffer(pRenderTarget->GetImageIndex());

		pCommandBuffer->BeginBufferRecording();
		pCommandBuffer->BindRenderTargetSecondary(pRenderTarget);

		pSecondaryCommandBuffer->BeginBufferRecording(pRenderTarget);

		mImGuiAdapter.Render(pSecondaryCommandBuffer);

		pSecondaryCommandBuffer->EndBufferRecording();

		pCommandBuffer->SubmitSecondaryCommandBuffer(pSecondaryCommandBuffer);
		pCommandBuffer->ExecuteSecondaryCommands();

		pCommandBuffer->UnbindRenderTarget();
		pCommandBuffer->EndBufferRecording();

		pRenderTarget->GetDevice()->SubmitGraphicsCommandBuffers({ pCommandBuffer });

		if (!pRenderTarget->PresentToDisplay())
			pRenderTarget->Recreate();

		pRenderTarget->IncrementFrameIndex();
		pRenderTarget->GetDevice()->WaitForQueue();
	}

	void EditorRenderTarget::UpdateUI(const UI64 delta)
	{
		mSceneRenderTarget.Render(pDisplay, delta);
	}
}
