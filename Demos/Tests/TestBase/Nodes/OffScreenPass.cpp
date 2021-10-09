// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "OffScreenPass.hpp"
#include "Engine/ProcessingPipeline/ProcessingPipeline.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

namespace Flint
{
	OffScreenPass::OffScreenPass(ProcessingPipeline* pProcessingPipeline)
		: ProcessingNode(pProcessingPipeline)
	{
		//pCommandBufferAllocator = pProcessingPipeline->GetCommandBufferAllocator()->CreateChildAllocator();
		//pCommandBuffers = pCommandBufferAllocator->CreateCommandBuffers();

		const auto pDevice = GetDevice();
		const auto frameExtent = GetExtent();

		pOffScreenRenderTarget = pDevice->CreateOffScreenRenderTarget(frameExtent, GetBufferCount(), CreateAttachments());
	}

	void OffScreenPass::Process(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 frameIndex, const UI32 imageIndex)
	{
		//auto pSecondaryCommandBuffer = pCommandBuffers[frameIndex];
		pCommandBuffer->BindRenderTarget(pOffScreenRenderTarget.get());

		// Draw all the game objects.
		for (auto const& pGameObject : pGameObjects)
			pGameObject->Draw(pCommandBuffer, frameIndex);

		pCommandBuffer->UnbindRenderTarget();
		//pCommandBuffer->CopyImage(pOffScreenRenderTarget->GetAttachment(0).pImage.get(), 0, pProcessingPipeline->GetColorBuffer().get(), 0);
	}

	std::vector<RenderTargetAttachment> OffScreenPass::CreateAttachments() const
	{
		const auto pDevice = GetDevice();
		const auto pDisplay = GetDisplay();
		const auto frameExtent = GetExtent();
		const FBox3D extent = FBox3D(frameExtent.mWidth, frameExtent.mHeight, 1);

		std::vector<RenderTargetAttachment> attachments = {};
		attachments.reserve(2);

		// Create the color image.
		attachments.emplace_back(
			RenderTargetAttachment(
				pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color | ImageUsage::Storage | ImageUsage::Graphics, extent, pDisplay->GetBestSwapChainFormat(pDevice.get()), 1, 1, nullptr),
				FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f)
			)
		);

		// Create the depth image.
		attachments.emplace_back(
			RenderTargetAttachment(
				pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Depth, extent, PixelFormat::D24_UNORMAL_S8_UINT, 1, 1, nullptr),
				DepthClearValues(1.0f, 0)
			)
		);

		return attachments;
	}
}
