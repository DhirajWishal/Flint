// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsScene.hpp"

#include "GraphicsCore/Instance.hpp"

namespace Flint
{
	GraphicsScene::GraphicsScene(ApplicationState* pState, const FBox2D extent)
		: pDevice(pState->pDevice)
	{
		auto pDisplay = pState->pInstance->CreateDisplay(extent, "Flint Test");

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
	}

	GraphicsScene::~GraphicsScene()
	{
		pRenderTarget->Terminate();
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
}