// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/ProcessingPipeline/ProcessingPipeline.hpp"

#include "GraphicsCore/CommandBufferAllocator.hpp"
#include "GraphicsCore/Instance.hpp"

namespace Flint
{
	ProcessingPipeline::ProcessingPipeline(const std::shared_ptr<Device>& pDevice, const FBox2D frameExtent, const std::string& displayTitle, const UI32 pipelineCount, const MultiSampleCount msaaCount, const bool forceColorBuffer, const bool forceDepthBuffer)
		: mMultiSampleCount(msaaCount)
	{
		UI32 bufferCount = pipelineCount;
		pDisplay = pDevice->GetInstance()->CreateDisplay(frameExtent, displayTitle);

		// Find the best buffer count if the pipeline count is 0.
		if (pipelineCount == 0)
			bufferCount = pDisplay->FindBestBufferCount(pDevice.get());

		const auto frameBufferExtent = pDisplay->GetExtent();
		const FBox3D extent = FBox3D(frameBufferExtent.mWidth, frameBufferExtent.mHeight, 1);
		std::vector<RenderTargetAttachment> attachments = {};

		// Check if we need a color buffer.
		if (mMultiSampleCount != MultiSampleCount::One || forceColorBuffer)
		{
			bContainsColorBuffer = true;
			attachments.emplace_back(
				RenderTargetAttachment(
					pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color, extent, pDisplay->GetBestSwapChainFormat(pDevice.get()), 1, 1, nullptr),
					FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f)
				)
			);
		}

		// Add the depth image if required.
		if (forceDepthBuffer)
		{
			bContainsDepthBuffer = true;
			attachments.emplace_back(
				RenderTargetAttachment(
					pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Depth, extent, PixelFormat::D24_UNORMAL_S8_UINT, 1, 1, nullptr),
					DepthClearValues(1.0f, 0)
				)
			);
		}

		// Create the screen bound render target.
		pScreenBoundRenderTarget = pDevice->CreateScreenBoundRenderTarget(pDisplay, frameBufferExtent, bufferCount, attachments, SwapChainPresentMode::MailBox);

		// Create the command buffer allocator and buffers.
		pCommandBufferAllocator = pDevice->CreateCommandBufferAllocator(bufferCount);
		pCommandBuffers = pCommandBufferAllocator->CreateCommandBuffers();
	}

	ProcessingPipeline::~ProcessingPipeline()
	{
		pScreenBoundRenderTarget->Terminate();
		pCommandBufferAllocator->Terminate();
		pDisplay->Terminate();
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

		// Begin the command buffer recording.
		pCommandBuffer->BeginBufferRecording();

		// Process all the nodes.
		for (auto const& pNode : pProcessingNodes)
			pNode->Process(pCommandBuffer, frameIndex, imageIndex);

		// Execute and end the buffer recording.
		pCommandBuffer->ExecuteSecondaryCommands();
		pCommandBuffer->EndBufferRecording();

		// Present the swap chain to the display.
		if (!pScreenBoundRenderTarget->PresentToDisplay())
			pScreenBoundRenderTarget->Recreate();

		// Increment the frame index.
		pScreenBoundRenderTarget->IncrementFrameIndex();
	}

	std::shared_ptr<Image> ProcessingPipeline::GetColorBuffer() const
	{
		// Get the image if the color attachment is available.
		if (IsColorImagePresent())
			return pScreenBoundRenderTarget->GetAttachments().front().pImage;

		return nullptr;
	}

	std::shared_ptr<Image> ProcessingPipeline::GetDepthBuffer() const
	{
		// Return if the depth image is not present.
		if (!IsDepthImagePresent())
			return nullptr;

		// Return the second image if the color attachment is present.
		if (IsColorImagePresent())
			return pScreenBoundRenderTarget->GetAttachment(1).pImage;

		// Return the first image.
		return pScreenBoundRenderTarget->GetAttachments().front().pImage;
	}
}
