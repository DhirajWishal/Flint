// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "FXAAPass.hpp"
#include "ScreenSpaceReflectionPass.hpp"
#include "Engine/ProcessingPipeline/ProcessingPipeline.hpp"
#include "Engine/ShaderCompiler.hpp"
#include "GraphicsCore/ResourcePackage.hpp"

#include <optick.h>

namespace Flint
{
	FXAAPass::FXAAPass(ProcessingPipeline* pProcessingPipeline, const OffScreenPass* pOffScreenPass)
		: ProcessingNode(pProcessingPipeline)
	{
		OPTICK_EVENT();

		const auto pDevice = GetDevice();
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
		const auto pOffScreenImage = pOffScreenPass->GetColorImage();
		pAntiAliasedImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Storage, pOffScreenImage->GetExtent(), pOffScreenImage->GetFormat(), pOffScreenImage->GetLayerCount(), pOffScreenImage->GetMipLevels(), nullptr);
		pScreenSpaceReflectionImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Graphics, pOffScreenImage->GetExtent(), pOffScreenImage->GetFormat(), pOffScreenImage->GetLayerCount(), pOffScreenImage->GetMipLevels(), nullptr);

		// Bind the resources.
		pResourcePackage->BindResource(0, pScreenSpaceReflectionImage, pScreenSpaceReflectionImage->CreateImageView(0, pScreenSpaceReflectionImage->GetLayerCount(), 0, pScreenSpaceReflectionImage->GetMipLevels(), ImageUsage::Graphics), pDevice->CreateImageSampler(ImageSamplerSpecification()));
		pResourcePackage->BindResource(1, pAntiAliasedImage, pAntiAliasedImage->CreateImageView(0, pAntiAliasedImage->GetLayerCount(), 0, pAntiAliasedImage->GetMipLevels(), ImageUsage::Storage), pDevice->CreateImageSampler(ImageSamplerSpecification()), ImageUsage::Storage);
	}

	void FXAAPass::Process(ProcessingNode* pPreviousNode, const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 frameIndex, const UI32 imageIndex)
	{
		OPTICK_EVENT();

		// Compute FXAA if enabled.
		if (bEnableFXAA)
		{
			pCommandBuffer->CopyImage(pPreviousNode->StaticCast<ScreenSpaceReflectionPass>().GetOutputImage().get(), 0, pScreenSpaceReflectionImage.get(), 0);

			pCommandBuffer->BindComputePipeline(pComputePipeline.get());
			pCommandBuffer->BindResourcePackage(pComputePipeline.get(), pResourcePackage.get());
			pCommandBuffer->IssueComputeCall(FBox3D(pAntiAliasedImage->GetExtent().mWidth / 32, pAntiAliasedImage->GetExtent().mHeight / 32, 1));
		}

		// Else copy to the anti aliased image.
		else
		{
			pCommandBuffer->CopyImage(pPreviousNode->StaticCast<ScreenSpaceReflectionPass>().GetOutputImage().get(), 0, pAntiAliasedImage.get(), 0);
		}
	}

	void FXAAPass::DrawUi()
	{
		OPTICK_EVENT();

		ImGui::Begin("FXAA");
		ImGui::Checkbox("Enable/ Disable", &bEnableFXAA);
		ImGui::End();
	}
}
