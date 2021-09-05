// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SceneRenderTarget.hpp"
#include "GraphicsCore/Device.hpp"
#include "GraphicsCore/Display.hpp"
#include "UI/ScenePanel.hpp"

namespace Flint
{
	void SceneRenderTarget::Initialize(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount, const FBox2D extent, const std::shared_ptr<ResourcePackage>& pPackage)
	{
		const FBox3D imageExtent = FBox3D(extent.mWidth, extent.mHeight, 1);
		const auto sampleCount = MultiSampleCount::One;
		//const auto sampleCount = pDevice->GetSupportedMultiSampleCount();

		mAttachments[0] = RenderTargetAttachment(
			pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color | ImageUsage::Graphics, imageExtent, PixelFormat::R8G8B8A8_UNORMAL, 1, 1, nullptr, sampleCount),
			FColor4D(CREATE_COLOR_256(32.0f), CREATE_COLOR_256(32.0f), CREATE_COLOR_256(32.0f), 1.0f));

		mAttachments[1] = RenderTargetAttachment(
			pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Depth, imageExtent, PixelFormat::D24_UNORMAL_S8_UINT, 1, 1, nullptr, sampleCount),
			DepthClearValues(1.0f, 0));

		pRenderTarget = pDevice->CreateOffScreenRenderTarget(extent, bufferCount, mAttachments);

		pSampler = pDevice->CreateImageSampler(ImageSamplerSpecification());
		pPackage->BindResource(0, pSampler, mAttachments[0].pImage);
		mTextureContainer.pResourcePackage = pPackage;

		pAllocator = pDevice->CreateCommandBufferAllocator(bufferCount);
		pSecondaryAllocator = pAllocator->CreateChildAllocator();

		pAllocator->CreateCommandBuffers();
		pSecondaryAllocator->CreateCommandBuffers();

		mVikingRoom.Initialize(pDevice, pRenderTarget, &mCamera);
		mCamera.SetAspectRatio(GetExtent());
	}

	void SceneRenderTarget::Terminate()
	{
		pRenderTarget->Terminate();

		for (auto& attachment : mAttachments)
			attachment.pImage->Terminate();

		pAllocator->Terminate();
		pSecondaryAllocator->Terminate();

		pSampler->Terminate();
		mVikingRoom.Terminate();
	}

	void SceneRenderTarget::Render(const std::shared_ptr<Display>& pDisplay, const UI64 delta)
	{
		mExtent.x = pDisplay->GetExtent().mWidth / 2;
		mExtent.y = pDisplay->GetExtent().mHeight / 2;

		const auto frameIndex = pRenderTarget->GetFrameIndex();
		auto pCommandBuffer = pAllocator->GetCommandBuffer(frameIndex);
		auto pSecondaryCommandBuffer = pSecondaryAllocator->GetCommandBuffer(frameIndex);

		pCommandBuffer->BeginBufferRecording();
		pCommandBuffer->BindRenderTargetSecondary(pRenderTarget);

		pSecondaryCommandBuffer->BeginBufferRecording(pRenderTarget);

		mVikingRoom.SubmitToCommandBuffer(pSecondaryCommandBuffer);

		pSecondaryCommandBuffer->EndBufferRecording();

		pCommandBuffer->SubmitSecondaryCommandBuffer(pSecondaryCommandBuffer);
		pCommandBuffer->ExecuteSecondaryCommands();

		pCommandBuffer->UnbindRenderTarget();
		pCommandBuffer->EndBufferRecording();

		pRenderTarget->GetDevice()->SubmitGraphicsCommandBuffers({ pCommandBuffer });
		pRenderTarget->IncrementFrameIndex();

		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize;

		// Draw the panel.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("Scene Panel", NULL, flags);
		ImGui::Image(reinterpret_cast<ImTextureID>(&mTextureContainer), mExtent);

		if (ImGui::IsItemHovered())
			UpdateCamera(pDisplay, delta);

		ImGui::End();
		ImGui::PopStyleVar(2);

		if (GetExtent() != pRenderTarget->GetExtent())
			Recreate(GetExtent());
	}

	void SceneRenderTarget::Recreate(const FBox2D& extent)
	{
		if (extent.IsZero())
			return;

		pRenderTarget->Recreate(extent);
		mTextureContainer.pResourcePackage->BindResource(0, pSampler, mAttachments[0].pImage);

		mVikingRoom.Recreate();
	}

	void SceneRenderTarget::UpdateCamera(const std::shared_ptr<Display>& pDisplay, const UI64 delta)
	{
		auto extent = pDisplay->GetExtent();
		if (!extent.IsZero())
			mCamera.SetAspectRatio(extent);

		// Update the camera.
		if (pDisplay->GetKeyEvent(KeyCode::KeyW).IsPressed() || pDisplay->GetKeyEvent(KeyCode::KeyW).IsOnRepeat())
			mCamera.MoveFront(delta);

		if (pDisplay->GetKeyEvent(KeyCode::KeyA).IsPressed() || pDisplay->GetKeyEvent(KeyCode::KeyA).IsOnRepeat())
			mCamera.MoveLeft(delta);

		if (pDisplay->GetKeyEvent(KeyCode::KeyS).IsPressed() || pDisplay->GetKeyEvent(KeyCode::KeyS).IsOnRepeat())
			mCamera.MoveBack(delta);

		if (pDisplay->GetKeyEvent(KeyCode::KeyD).IsPressed() || pDisplay->GetKeyEvent(KeyCode::KeyD).IsOnRepeat())
			mCamera.MoveRight(delta);

		if (pDisplay->GetMouseButtonEvent(MouseButton::Left).IsPressed())
			mCamera.MousePosition(pDisplay->GetMousePosition());

		if (pDisplay->GetMouseButtonEvent(MouseButton::Left).IsReleased())
			mCamera.ResetFirstMouse();

		mCamera.Update(delta);
	}
}