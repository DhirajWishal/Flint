// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "FXAAPass.hpp"
#include "Engine/ProcessingPipeline/ProcessingPipeline.hpp"
#include "Engine/ShaderCompiler.hpp"
#include "GraphicsCore/ResourcePackage.hpp"

namespace Flint
{
	FXAAPass::FXAAPass(ProcessingPipeline* pProcessingPipeline, OffScreenPass* pOffScreenPass)
		: ProcessingNode(pProcessingPipeline), pOffScreenPass(pOffScreenPass)
	{
		const auto pDevice = GetDevice();
		const auto pOffScreenImage = pOffScreenPass->GetColorImage();

		std::shared_ptr<Shader> pShader = nullptr;

		// Check if we have to compile the source. If so compile.
		if (!std::filesystem::exists("Flint/Shaders/fxaa.comp.fsc"))
		{
			ShaderCompiler compiler(std::filesystem::path("Shaders/FXAA/fxaa.comp"), ShaderCodeType::GLSL, ShaderType::Compute);
			pShader = compiler.CreateShader(pDevice);
			pShader->CreateCache("Flint/Shaders/fxaa.comp.fsc");
		}
		else
			pShader = pDevice->CreateShader(ShaderType::Compute, std::filesystem::path("Flint/Shaders/fxaa.comp.fsc"));

		// Create the compute pipeline.
		pComputePipeline = pDevice->CreateComputePipeline("FXAAPipeline", pShader);
		pResourcePackage = pComputePipeline->CreateResourcePackage(0);

		// Create the anti aliasing image.
		pAntiAliasedImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Storage, pOffScreenImage->GetExtent(), pOffScreenImage->GetFormat(), pOffScreenImage->GetLayerCount(), pOffScreenImage->GetMipLevels(), nullptr);

		// Bind the resources.
		pResourcePackage->BindResource(0, pOffScreenImage, pOffScreenImage->CreateImageView(0, pOffScreenImage->GetLayerCount(), 0, pOffScreenImage->GetMipLevels(), ImageUsage::Graphics), pDevice->CreateImageSampler(ImageSamplerSpecification()));
		pResourcePackage->BindResource(1, pAntiAliasedImage, pAntiAliasedImage->CreateImageView(0, pAntiAliasedImage->GetLayerCount(), 0, pAntiAliasedImage->GetMipLevels(), ImageUsage::Storage), pDevice->CreateImageSampler(ImageSamplerSpecification()), ImageUsage::Storage);
	}

	void FXAAPass::Process(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 frameIndex, const UI32 imageIndex)
	{
		pCommandBuffer->BindRenderTarget(pProcessingPipeline->GetScreenBoundRenderTarget().get());
		pCommandBuffer->UnbindRenderTarget();

		// Compute FXAA if enabled.
		if (bEnableFXAA)
		{
			pCommandBuffer->BindComputePipeline(pComputePipeline.get());
			pCommandBuffer->BindResourcePackage(pComputePipeline.get(), pResourcePackage.get());

			const auto workGroup = FBox3D(pAntiAliasedImage->GetExtent().mWidth / 32, pAntiAliasedImage->GetExtent().mHeight / 32, 1);
			pCommandBuffer->IssueComputeCall(workGroup);

			// Get the color image of the processing pipeline.
			const auto pColorImage = pProcessingPipeline->GetColorBuffer();

			// If the color image is present, copy the color image from the off screen pass to it. Else copy the image to the swap chain.
			if (pColorImage)
				pCommandBuffer->CopyImage(pAntiAliasedImage.get(), 0, pColorImage.get(), 0);
			else
				pCommandBuffer->CopyToSwapChainImage(pAntiAliasedImage.get(), 0, pProcessingPipeline->GetScreenBoundRenderTarget()->GetSwapChain().get(), imageIndex, 0);
		}

		// If not just copy the off screen image.
		else
		{
			// Get the color image of the processing pipeline.
			const auto pColorImage = pProcessingPipeline->GetColorBuffer();
			const auto pOffScreenImage = pOffScreenPass->GetColorImage();

			// If the color image is present, copy the color image from the off screen pass to it. Else copy the image to the swap chain.
			if (pColorImage)
				pCommandBuffer->CopyImage(pOffScreenImage.get(), 0, pColorImage.get(), 0);
			else
				pCommandBuffer->CopyToSwapChainImage(pOffScreenImage.get(), 0, pProcessingPipeline->GetScreenBoundRenderTarget()->GetSwapChain().get(), imageIndex, 0);
		}
	}

	void FXAAPass::DrawUi()
	{
		ImGui::Begin("FXAA");
		ImGui::Checkbox("Enable/ Disable", &bEnableFXAA);
		ImGui::End();
	}
}
