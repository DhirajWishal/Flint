// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsScene.hpp"

#include "GraphicsCore/Instance.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

namespace Flint
{
	GraphicsScene::GraphicsScene(Application* pApplication, FBox2D extent)
		: pDevice(pApplication->pDevice)
	{
		auto pDisplay = pApplication->pInstance->CreateDisplay(extent, "Flint Test");
		extent = pDisplay->GetExtent();

		std::vector<RenderTargetAttachment> attachments(2);
		const auto sampleCount = pDevice->GetSupportedMultiSampleCount();
		const FBox3D imageExtent = FBox3D(extent.mWidth, extent.mHeight, 1);

		attachments[0] = RenderTargetAttachment(
			pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color, imageExtent, pDisplay->GetBestSwapChainFormat(pDevice), 1, 1, nullptr, sampleCount),
			FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f));

		attachments[1] = RenderTargetAttachment(
			pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Depth, imageExtent, PixelFormat::D24_UNORMAL_S8_UINT, 1, 1, nullptr, sampleCount),
			DepthClearValues(1.0f, 0));

		pRenderTarget = pDevice->CreateScreenBoundRenderTarget(pDisplay, extent, pDisplay->FindBestBufferCount(pDevice), attachments, SwapChainPresentMode::MailBox);

		pAllocator = pDevice->CreateCommandBufferAllocator(pRenderTarget->GetBufferCount());
		pSecondaryAllocator = pAllocator->CreateChildAllocator();

		pCommandBuffers = pAllocator->CreateCommandBuffers();
		pSecondaryCommandBuffers = pSecondaryAllocator->CreateCommandBuffers();

		pSynchronizationPrimtives.reserve(pSecondaryCommandBuffers.size());
		for (UI64 i = 0; i < pCommandBuffers.size(); i++)
			pSynchronizationPrimtives.push_back(pDevice->CreateSynchronizationPrimitive());

		mImGuiAdapter.Initialize(pDevice, pRenderTarget);
		mCamera.SetAspectRatio(pDisplay->GetExtent());
	}

	void GraphicsScene::Terminate()
	{
		pRenderTarget->Terminate();

		pAllocator->Terminate();
		pSecondaryAllocator->Terminate();

		mImGuiAdapter.Terminate();
	}

	std::shared_ptr<GraphicsPipeline> GraphicsScene::CreateGraphicsPipeline(const std::string& name, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pFragmentShader, GraphicsPipelineSpecification spec)
	{
		std::shared_ptr<GraphicsPipeline> pPipeline = pDevice->CreateGraphicsPipeline(name, pRenderTarget, pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, spec);
		pGraphicsPipelines[name] = pPipeline;

		return pPipeline;
	}

	std::shared_ptr<GraphicsPipeline> GraphicsScene::GetGraphicsPipeline(const std::string& name) const
	{
		if (pGraphicsPipelines.find(name) == pGraphicsPipelines.end())
			return nullptr;

		return pGraphicsPipelines.at(name);
	}

	void GraphicsScene::SubmitAssetsToDraw(const std::string& name, const std::shared_ptr<GraphicsPipeline>& pPipeline, const Asset& asset)
	{
		mDrawData[name] = DrawDataContainer(pPipeline, asset);
	}

	void GraphicsScene::Update()
	{
		auto currentTimePoint = mClock.now();
		UI64 delta = currentTimePoint.time_since_epoch().count() - mOldTimePoint.time_since_epoch().count();

		auto pDisplay = GetDisplay();
		pDisplay->Update();

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = delta / 1000.0f;

		ImGui::Begin("Statistics");
		ImGui::Text("Frame time: %.4f ms", io.DeltaTime / 1000.0f);
		ImGui::End();

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

		for (auto pObject : pGameObjects)
			pObject->Update(0, &mCamera);

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

		mCamera.Update();

		mOldTimePoint = currentTimePoint;
	}

	void GraphicsScene::DrawFrame()
	{
		if (!pRenderTarget->PrepareNewFrame())
		{
			pRenderTarget->Recreate();
			return;
		}

		auto pSynchronizationPrimtive = pSynchronizationPrimtives[pRenderTarget->GetImageIndex()];
		pSynchronizationPrimtive->Wait();
		auto pCommandBuffer = pAllocator->GetCommandBuffer(pRenderTarget->GetImageIndex());
		auto pSecondaryCommandBuffer = pSecondaryAllocator->GetCommandBuffer(pRenderTarget->GetImageIndex());

		pCommandBuffer->BeginBufferRecording();
		pCommandBuffer->BindRenderTargetSecondary(pRenderTarget);

		pSecondaryCommandBuffer->BeginBufferRecording(pRenderTarget);

		for (auto pObject : pGameObjects)
			pObject->Draw(pSecondaryCommandBuffer);

		mImGuiAdapter.Render(pSecondaryCommandBuffer);

		pSecondaryCommandBuffer->EndBufferRecording();

		pCommandBuffer->SubmitSecondaryCommandBuffer(pSecondaryCommandBuffer);
		pCommandBuffer->ExecuteSecondaryCommands();

		pCommandBuffer->UnbindRenderTarget();
		pCommandBuffer->EndBufferRecording();

		pRenderTarget->GetDevice()->SubmitGraphicsCommandBuffers({ pCommandBuffer }, pSynchronizationPrimtive);

		if (!pRenderTarget->PresentToDisplay())
			pRenderTarget->Recreate();

		pRenderTarget->IncrementFrameIndex();
		//pRenderTarget->GetDevice()->WaitForQueue();
	}
}