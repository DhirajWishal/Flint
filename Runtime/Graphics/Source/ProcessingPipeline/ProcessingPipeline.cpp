// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/ProcessingPipeline/ProcessingPipeline.hpp"

#include "GraphicsCore/CommandBufferAllocator.hpp"
#include "GraphicsCore/Instance.hpp"
#include "GraphicsCore/HostSynchronizationPrimitive.hpp"

namespace Flint
{
	ProcessingPipeline::ProcessingPipeline(Device* pDevice, const FBox2D frameExtent, const std::string& displayTitle, const uint32_t pipelineCount, const MultiSampleCount msaaCount, const bool forceColorBuffer, const bool forceDepthBuffer)
		: m_MultiSampleCount(msaaCount)
	{
		uint32_t bufferCount = pipelineCount;
		pDisplay = pDevice->GetInstance()->CreateDisplay(frameExtent, displayTitle);

		// Find the best buffer count if the pipeline count is 0.
		if (pipelineCount == 0)
			bufferCount = pDisplay->FindBestBufferCount(pDevice);

		const auto frameBufferExtent = pDisplay->GetExtent();
		const FBox3D extent = FBox3D(frameBufferExtent.m_Width, frameBufferExtent.m_Height, 1);
		std::vector<RenderTargetAttachment> attachments = {};

		// Check if we need a color buffer.
		if (m_MultiSampleCount != MultiSampleCount::One || forceColorBuffer)
		{
			bContainsColorBuffer = true;

			auto pImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color, extent, pDisplay->GetBestSwapChainFormat(pDevice), 1, 1, nullptr);
			attachments.emplace_back(
				RenderTargetAttachment(
					pImage.get(),
					FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f)
				)
			);

			pImageAttachments.emplace_back(std::move(pImage));
		}

		// Add the depth image if required.
		if (forceDepthBuffer)
		{
			bContainsDepthBuffer = true;

			auto pImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Depth, extent, PixelFormat::D24_UNORMAL_S8_UINT, 1, 1, nullptr);
			attachments.emplace_back(
				RenderTargetAttachment(
					pImage.get(),
					DepthClearValues(1.0f, 0)
				)
			);

			pImageAttachments.emplace_back(std::move(pImage));
		}

		// Create the screen bound render target.
		pScreenBoundRenderTarget = pDevice->CreateScreenBoundRenderTarget(pDisplay.get(), frameBufferExtent, bufferCount, attachments, SwapChainPresentMode::MailBox);

		// Create the command buffer allocator and buffers.
		pCommandBufferAllocator = pDevice->CreateCommandBufferAllocator(bufferCount);
		pCommandBuffers = pCommandBufferAllocator->CreateCommandBuffers();

		// Create the synchronization primitives.
		pHostSynchronizationPrimitives.reserve(bufferCount);
		for (uint32_t i = 0; i < bufferCount; i++)
			pHostSynchronizationPrimitives.emplace_back(pDevice->CreateHostSynchronizationPrimitive());
	}

	ProcessingPipeline::~ProcessingPipeline()
	{
		pScreenBoundRenderTarget->Terminate();
		pCommandBufferAllocator->Terminate();
		pDisplay->Terminate();

		pHostSynchronizationPrimitives.clear();
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

		// Get the synchronization primitive and wait until the command buffer is completed.
		const auto pHostSynchronizationPrimitives = GetInFlightHostSynchronizationPrimitive();
		pHostSynchronizationPrimitives->Wait();

		// Begin the command buffer recording.
		pCommandBuffer->BeginBufferRecording();

		// Process all the nodes.
		ProcessNodes(pCommandBuffer, frameIndex, imageIndex);

		// Execute and end the buffer recording.
		pCommandBuffer->ExecuteSecondaryCommands();
		pCommandBuffer->EndBufferRecording();

		// Submit the commands to the device.
		GetDevice()->SubmitGraphicsCommandBuffer(pCommandBuffer.get(), pHostSynchronizationPrimitives);

		// Present the swap chain to the display.
		if (!pScreenBoundRenderTarget->PresentToDisplay())
			pScreenBoundRenderTarget->Recreate();

		// Increment the frame index.
		pScreenBoundRenderTarget->IncrementFrameIndex();
	}

	Image* ProcessingPipeline::GetColorBuffer() const
	{
		// Get the image if the color attachment is available.
		if (IsColorImagePresent())
			return pScreenBoundRenderTarget->GetAttachments().front().pImage;

		return nullptr;
	}

	Image* ProcessingPipeline::GetDepthBuffer() const
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

	void ProcessingPipeline::ProcessNodes(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32_t frameIndex, const uint32_t imageIndex)
	{
		ProcessingNode* pPreviousNode = nullptr;
		for (const auto& pNode : pProcessingNodes)
		{
			pNode->Process(pPreviousNode, pCommandBuffer, frameIndex, imageIndex);
			pPreviousNode = pNode.get();
		}
	}
}
