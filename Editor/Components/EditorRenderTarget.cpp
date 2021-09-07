// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "EditorRenderTarget.hpp"

#include "GraphicsCore/Instance.hpp"

#include "Engine/Client/Components/DrawInstance.hpp"

namespace Flint
{
	EditorRenderTarget::EditorRenderTarget()
	{
	}

	EditorRenderTarget::~EditorRenderTarget()
	{
	}

	void EditorRenderTarget::Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Instance>& pInstnace)
	{
		pDisplay = pInstnace->CreateDisplay({ std::numeric_limits<UI32>::max() }, "Flint Editor");
		mExtent = pDisplay->GetExtent();

		const auto sampleCount = pDevice->GetSupportedMultiSampleCount();
		const FBox3D imageExtent = FBox3D(mExtent.mWidth, mExtent.mHeight, 1);

		mAttachments[0] = RenderTargetAttachment(
			pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color, imageExtent, pDisplay->GetBestSwapChainFormat(pDevice), 1, 1, nullptr, sampleCount),
			FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f));

		mAttachments[1] = RenderTargetAttachment(
			pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Depth, imageExtent, PixelFormat::D24_UNORMAL_S8_UINT, 1, 1, nullptr, sampleCount),
			DepthClearValues(1.0f, 0));

		pRenderTarget = pDevice->CreateScreenBoundRenderTarget(pDisplay, mExtent, pDisplay->FindBestBufferCount(pDevice), mAttachments, SwapChainPresentMode::MailBox);

		pAllocator = pDevice->CreateCommandBufferAllocator(pRenderTarget->GetBufferCount());
		pSecondaryAllocator = pAllocator->CreateChildAllocator();

		pAllocator->CreateCommandBuffers();
		pSecondaryAllocator->CreateCommandBuffers();

		mImGuiAdapter.Initialize(pDevice, pRenderTarget);
	}

	void EditorRenderTarget::Terminate()
	{
		//mSceneRenderTarget.Terminate();
		pRenderTarget->Terminate();

		pAllocator->Terminate();
		pSecondaryAllocator->Terminate();
		pDisplay->Terminate();

		mImGuiAdapter.Terminate();

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
			mExtent = extent;
			io.DisplaySize = ImVec2(static_cast<float>(extent.mWidth), static_cast<float>(extent.mHeight));
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
	}

	void EditorRenderTarget::DrawFrame(const ClientInterface* pClient)
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

		// Draw the client data.
		if (pClient)
		{
			// Draw the globals.
			auto pGlobalComponentStore = pClient->FindGlobalComponentStore<Components::DrawInstanceGraphics>();
			if (pGlobalComponentStore)
			{
				for (auto& component : pGlobalComponentStore->GetArray())
				{
					pSecondaryCommandBuffer->BindGeometryStore(component.pGeometryStore);
					pSecondaryCommandBuffer->BindGraphicsPipeline(component.pGraphicsPipeline);

					for (auto& drawData : component.mDrawData)
					{
						pSecondaryCommandBuffer->BindDynamicStates(component.pGraphicsPipeline, drawData.pDynamicStateContainer);
						pSecondaryCommandBuffer->BindResourcePackages(component.pGraphicsPipeline, drawData.pResourcePackages);
						pSecondaryCommandBuffer->IssueDrawCall(*drawData.pWireFrame);
					}
				}
			}

			// Draw the privates.
			auto& componentMap = pClient->GetComponentMap();
			for (auto& controller : componentMap)
			{
				auto pComponentStore = pClient->FindComponentStore<Components::DrawInstanceGraphics>(controller.first);
				if (pComponentStore)
				{
					for (auto& component : pComponentStore->GetArray())
					{
						pSecondaryCommandBuffer->BindGeometryStore(component.pGeometryStore);
						pSecondaryCommandBuffer->BindGraphicsPipeline(component.pGraphicsPipeline);

						for (auto& drawData : component.mDrawData)
						{
							pSecondaryCommandBuffer->BindDynamicStates(component.pGraphicsPipeline, drawData.pDynamicStateContainer);
							pSecondaryCommandBuffer->BindResourcePackages(component.pGraphicsPipeline, drawData.pResourcePackages);
							pSecondaryCommandBuffer->IssueDrawCall(*drawData.pWireFrame);
						}
					}
				}
			}
		}

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
	}

	const std::vector<std::filesystem::path> EditorRenderTarget::GetDropPaths()
	{
		return pDisplay->GetDragAndDropValues();
	}
}
