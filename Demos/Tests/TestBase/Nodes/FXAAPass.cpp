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
		pResourcePackage->BindResource(0, pDevice->CreateImageSampler(ImageSamplerSpecification()), pOffScreenImage);
		pResourcePackage->BindResource(1, pDevice->CreateImageSampler(ImageSamplerSpecification()), pAntiAliasedImage, ImageUsage::Storage);
	}

	void FXAAPass::Process(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 frameIndex, const UI32 imageIndex)
	{
		pCommandBuffer->BindRenderTarget(pProcessingPipeline->GetScreenBoundRenderTarget().get());
		pCommandBuffer->UnbindRenderTarget();

		pCommandBuffer->BindComputePipeline(pComputePipeline.get());
		pCommandBuffer->BindResourcePackage(pComputePipeline.get(), pResourcePackage.get());
		pCommandBuffer->IssueComputeCall(FBox3D(pAntiAliasedImage->GetExtent().mWidth / 32, pAntiAliasedImage->GetExtent().mHeight / 32, 1));

		// Get the color image of the processing pipeline.
		const auto pColorImage = pProcessingPipeline->GetColorBuffer();

		// If the color image is present, copy the color image from the off screen pass to it. Else copy the image to the swap chain.
		if (pColorImage)
			pCommandBuffer->CopyImage(pAntiAliasedImage.get(), 0, pColorImage.get(), 0);
		else
			pCommandBuffer->CopyToSwapChainImage(pAntiAliasedImage.get(), 0, pProcessingPipeline->GetScreenBoundRenderTarget()->GetSwapChain().get(), imageIndex, 0);
	}
}
