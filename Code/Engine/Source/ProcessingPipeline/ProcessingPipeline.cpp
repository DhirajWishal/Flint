// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/ProcessingPipeline/ProcessingPipeline.hpp"

#include "GraphicsCore/CommandBufferAllocator.hpp"
#include "GraphicsCore/Instance.hpp"

namespace Flint
{
	ProcessingPipeline::ProcessingPipeline(const std::shared_ptr<Device>& pDevice, const FBox2D frameExtent, const std::string& displayTitle, const UI32 pipelineCount, const MultiSampleCount msaaCount, const bool forceColorBuffer)
		: mMultiSampleCount(msaaCount)
	{
		pDisplay = pDevice->GetInstance()->CreateDisplay(frameExtent, displayTitle);

		const FBox3D extent = FBox3D(frameExtent.mWidth, frameExtent.mHeight, 1);
		std::vector<RenderTargetAttachment> attachments = {};

		// Check if we need a color buffer.
		if (mMultiSampleCount != MultiSampleCount::One || forceColorBuffer)
		{
			bContainsColorBuffer = true;
			attachments.emplace_back(
				RenderTargetAttachment(
					pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color, extent, pDisplay->GetBestSwapChainFormat(pDevice.get()), 1, 1, nullptr),
					FColor4D()
				)
			);
		}

		// Add the depth image.
		//attachments.emplace_back(
		//	RenderTargetAttachment(
		//		pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Depth, extent, PixelFormat::D24_UNORMAL_S8_UINT, 1, 1, nullptr),
		//		FColor4D()
		//	)
		//);

		// Create the screen bound render target.
		pScreenBoundRenderTarget = pDevice->CreateScreenBoundRenderTarget(pDisplay, frameExtent, pipelineCount, attachments, SwapChainPresentMode::MailBox);
	}

	ProcessingPipeline::~ProcessingPipeline()
	{
		pScreenBoundRenderTarget->Terminate();
		pDisplay->Terminate();
		pCommandBufferAllocator->Terminate();
	}

	void ProcessingPipeline::Execute()
	{
		// Prepare the new frame.
		if (!pScreenBoundRenderTarget->PrepareNewFrame())
		{
			pScreenBoundRenderTarget->Recreate();
			return;
		}

		// Prepare the required resources.
		const auto pCommandBuffer = GetCommandBufferInFlight();
		const auto frameIndex = GetCurrentFrameIndex();
		const auto imageIndex = GetCurrentImageIndex();

		// Process all the nodes.
		for (auto const& pNode : pProcessingNodes)
			pNode->Process(pCommandBuffer, frameIndex, imageIndex);

		// Present the swap chain to the display.
		if (!pScreenBoundRenderTarget->PresentToDisplay())
			pScreenBoundRenderTarget->Recreate();

		// Increment the frame index.
		pScreenBoundRenderTarget->IncrementFrameIndex();
	}

	std::shared_ptr<Image> ProcessingPipeline::GetColorBuffer() const
	{
		if (IsColorImagePresent())
			return pScreenBoundRenderTarget->GetAttachments().front().pImage;

		return nullptr;
	}
}
