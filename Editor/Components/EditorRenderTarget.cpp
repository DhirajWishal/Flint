// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "EditorRenderTarget.hpp"

#include "GraphicsCore/Instance.hpp"

void EditorRenderTarget::Initialize(const std::shared_ptr<Flint::Device>& pDevice, const std::shared_ptr<Flint::Instance>& pInstnace)
{
	const Flint::FBox2D extent = Flint::FBox2D(1280, 720);
	pDisplay = pInstnace->CreateDisplay(extent, "Flint Editor");

	Flint::FBox3D imageExtent = Flint::FBox3D(extent.mWidth, extent.mHeight, 1);
	const auto sampleCount = pDevice->GetSupportedMultiSampleCount();

	mAttachments[0] = Flint::RenderTargetAttachment(
		pDevice->CreateImage(Flint::ImageType::TwoDimension, Flint::ImageUsage::Color, imageExtent, pDisplay->GetBestSwapChainFormat(pDevice), 1, 1, nullptr, sampleCount),
		Flint::FColor4D(CREATE_COLOR_256(32.0f), CREATE_COLOR_256(32.0f), CREATE_COLOR_256(32.0f), 1.0f));

	mAttachments[1] = Flint::RenderTargetAttachment(
		pDevice->CreateImage(Flint::ImageType::TwoDimension, Flint::ImageUsage::Depth, imageExtent, Flint::PixelFormat::D24_UNORMAL_S8_UINT, 1, 1, nullptr, sampleCount),
		Flint::DepthClearValues(1.0f, 0));

	pRenderTarget = pDevice->CreateScreenBoundRenderTarget(pDisplay, extent, pDisplay->FindBestBufferCount(pDevice), mAttachments, Flint::SwapChainPresentMode::MailBox);

	pAllocator = pDevice->CreateCommandBufferAllocator(pRenderTarget->GetBufferCount());
	pSecondaryAllocator = pAllocator->CreateChildAllocator();

	pAllocator->CreateCommandBuffers();
	pSecondaryAllocator->CreateCommandBuffers();

	mImGuiAdapter.Initialize(pDevice, pRenderTarget);
}

void EditorRenderTarget::Terminate()
{
	pAllocator->Terminate();
	pSecondaryAllocator->Terminate();
	pRenderTarget->Terminate();
	pDisplay->Terminate();

	for (auto& attachment : mAttachments)
		attachment.pImage->Terminate();

	mImGuiAdapter.Terminate();
}

bool EditorRenderTarget::IsDisplayOpen() const
{
	return pDisplay->IsOpen();
}

void EditorRenderTarget::PollEvents()
{
	pDisplay->Update();
}

void EditorRenderTarget::DrawFrame()
{
	pRenderTarget->PrepareNewFrame();
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
	pRenderTarget->PresentToDisplay();
	pRenderTarget->IncrementFrameIndex();
	pRenderTarget->GetDevice()->WaitForQueue();
}
