// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImGuiPass.hpp"
#include "FXAAPass.hpp"
#include "Graphics/ProcessingPipeline/ProcessingPipeline.hpp"

#include <optick.h>

namespace Flint
{
	ImGuiPass::ImGuiPass(ProcessingPipeline* pProcessingPipeline)
		: ProcessingNode(pProcessingPipeline)
	{
		OPTICK_EVENT();

		const auto pDevice = GetDevice();
		const auto frameExtent = GetExtent();

		pOffScreenRenderTarget = pDevice->CreateOffScreenRenderTarget(frameExtent, GetBufferCount(), CreateAttachments());
		mImGuiAdapter.Initialize(pDevice, pOffScreenRenderTarget);

		mTextureContainer.pResourcePackage = mImGuiAdapter.CreateResourcePackage();

		auto pColorImage = pOffScreenRenderTarget->GetAttachment(0).pImage;
		mTextureContainer.pResourcePackage->BindResource(0, pColorImage, pColorImage->CreateImageView(0, pColorImage->GetLayerCount(), 0, pColorImage->GetMipLevels(), ImageUsage::Graphics), pDevice->CreateImageSampler(ImageSamplerSpecification()));
		mTextureContainer.mExtent = ImVec2(static_cast<float>(pColorImage->GetExtent().mWidth), static_cast<float>(pColorImage->GetExtent().mHeight));
	}

	void ImGuiPass::Process(ProcessingNode* pPreviousNode, const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 frameIndex, const uint32 imageIndex)
	{
		OPTICK_EVENT();

		pCommandBuffer->CopyImage(pPreviousNode->StaticCast<FXAAPass>().GetAntiAliasedImage().get(), 0, pOffScreenRenderTarget->GetAttachment(0).pImage.get(), 0);
		pCommandBuffer->BindRenderTarget(pOffScreenRenderTarget.get());

		mImGuiAdapter.Render(pCommandBuffer, frameIndex);
		pCommandBuffer->UnbindRenderTarget();

		// Get the color image of the processing pipeline.
		const auto pColorImage = pProcessingPipeline->GetColorBuffer();
		const auto pOutputImage = pOffScreenRenderTarget->GetAttachment(0).pImage;

		// If the color image is present, copy the color image from the off screen pass to it. Else copy the image to the swap chain.
		if (pColorImage)
			pCommandBuffer->CopyImage(pOutputImage.get(), 0, pColorImage.get(), 0);
		else
			pCommandBuffer->CopyToSwapChainImage(pOutputImage.get(), 0, pProcessingPipeline->GetScreenBoundRenderTarget()->GetSwapChain().get(), imageIndex, 0);

		// Include the swap chain as an external one.
		pCommandBuffer->IncludeSwapChain(pProcessingPipeline->GetScreenBoundRenderTarget()->GetSwapChain().get());
	}

	std::vector<RenderTargetAttachment> ImGuiPass::CreateAttachments() const
	{
		OPTICK_EVENT();

		const auto pDevice = GetDevice();
		const auto pDisplay = GetDisplay();
		const auto frameExtent = GetExtent();
		const FBox3D extent = FBox3D(frameExtent.mWidth, frameExtent.mHeight, 1);

		std::vector<RenderTargetAttachment> attachments = {};
		auto pColorImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color | ImageUsage::Storage | ImageUsage::Graphics, extent, pDisplay->GetBestSwapChainFormat(pDevice.get()), 1, 1, nullptr);

		auto& imageSpec = pColorImage->GetImageRenderTargetSpecification();
		imageSpec = imageSpec | ImageRenderTargetSpecification::DiscardLoad;

		// Create the color image.
		attachments.emplace_back(
			RenderTargetAttachment(
				pColorImage,
				FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f)
			)
		);

		return attachments;
	}
}
