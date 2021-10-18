// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "OffScreenPass.hpp"
#include "Graphics/ProcessingPipeline/ProcessingPipeline.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"
#include "GraphicsCore/Query.hpp"

#include <optick.h>

namespace Flint
{
	OffScreenPass::OffScreenPass(ProcessingPipeline* pProcessingPipeline)
		: ProcessingNode(pProcessingPipeline)
	{
		OPTICK_EVENT();

		//pCommandBufferAllocator = pProcessingPipeline->GetCommandBufferAllocator()->CreateChildAllocator();
		//pCommandBuffers = pCommandBufferAllocator->CreateCommandBuffers();

		const auto pDevice = GetDevice();
		const auto frameExtent = GetExtent();

		pOffScreenRenderTarget = pDevice->CreateOffScreenRenderTarget(frameExtent, GetBufferCount(), CreateAttachments());
		bIsFirstPass.resize(GetBufferCount(), true);
	}

	void OffScreenPass::Process(ProcessingNode* pPreviousNode, const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 frameIndex, const UI32 imageIndex)
	{
		OPTICK_EVENT();

		// Reset the occlusion queries.
		for (const auto& pGameObject : pGameObjects)
			pGameObject->ResetOcclusionQuery(pCommandBuffer, frameIndex, bIsFirstPass[frameIndex]);

		bIsFirstPass[frameIndex] = false;

		//auto pSecondaryCommandBuffer = pCommandBuffers[frameIndex];
		pCommandBuffer->BindRenderTarget(pOffScreenRenderTarget.get());

		// Occlusion pass.
		UI32 index = 0;
		for (const auto& pGameObject : pGameObjects)
			pGameObject->OcclusionPass(pCommandBuffer, frameIndex);

		pCommandBuffer->ClearRenderTarget(pOffScreenRenderTarget.get());

		// Draw all the game objects.
		for (const auto& pGameObject : pGameObjects)
			pGameObject->Draw(pCommandBuffer, frameIndex);

		pCommandBuffer->UnbindRenderTarget();
		//pCommandBuffer->CopyImage(pOffScreenRenderTarget->GetAttachment(0).pImage.get(), 0, pProcessingPipeline->GetColorBuffer().get(), 0);
	}

	std::vector<RenderTargetAttachment> OffScreenPass::CreateAttachments() const
	{
		OPTICK_EVENT();

		const auto pDevice = GetDevice();
		const auto pDisplay = GetDisplay();
		const auto frameExtent = GetExtent();
		const FBox3D extent = FBox3D(frameExtent.mWidth, frameExtent.mHeight, 1);

		std::vector<RenderTargetAttachment> attachments = {};

		// Create the color image 0.
		attachments.emplace_back(
			RenderTargetAttachment(
				pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color | ImageUsage::Storage | ImageUsage::Graphics, extent, pDisplay->GetBestSwapChainFormat(pDevice.get()), 1, 1, nullptr),
				FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f)
			)
		);

		// Create the color image 1.
		//attachments.emplace_back(
		//	RenderTargetAttachment(
		//		pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color | ImageUsage::Storage | ImageUsage::Graphics, extent, PixelFormat::R16G16B16A16_SFLOAT, 1, 1, nullptr),
		//		FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f)
		//	)
		//);

		// Create the color image 2.
		attachments.emplace_back(
			RenderTargetAttachment(
				pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color | ImageUsage::Storage | ImageUsage::Graphics, extent, PixelFormat::R16G16B16A16_SFLOAT, 1, 1, nullptr),
				FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f)
			)
		);

		// Create the depth image.
		attachments.emplace_back(
			RenderTargetAttachment(
				pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Depth | ImageUsage::Graphics, extent, PixelFormat::D32_SFLOAT_S8_UINT, 1, 1, nullptr),
				DepthClearValues(1.0f, 0)
			)
		);

		return attachments;
	}
}
