// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "EditorRenderTarget.hpp"

void EditorRenderTarget::Initialize(const std::shared_ptr<Flint::Device>& pDevice, const std::shared_ptr<Flint::Display>& pDisplay)
{
	const auto displayExtent = pDisplay->GetExtent();
	Flint::FBox3D imageExtent = Flint::FBox3D(displayExtent.mWidth, displayExtent.mHeight, 1);

	std::vector<Flint::RenderTargetAttachment> attachments(2);
	attachments[0] = Flint::RenderTargetAttachment(
		pDevice->CreateImage(Flint::ImageType::DIMENSIONS_2, Flint::ImageUsage::COLOR, imageExtent, pDisplay->GetBestSwapChainFormat(pDevice), 1, 1, nullptr),
		Flint::FColor4D(CREATE_COLOR_256(32.0f), CREATE_COLOR_256(32.0f), CREATE_COLOR_256(32.0f), 1.0f));

	attachments[1] = Flint::RenderTargetAttachment(
		pDevice->CreateImage(Flint::ImageType::DIMENSIONS_2, Flint::ImageUsage::DEPTH, imageExtent, Flint::PixelFormat::D16_SINT, 1, 1, nullptr),
		Flint::DepthClearValues(1.0f, 0));

	pRenderTarget = pDevice->CreateScreenBoundRenderTarget(pDisplay, displayExtent, pDisplay->FindBestBufferCount(pDevice), attachments, Flint::SwapChainPresentMode::MAILBOX);
}
