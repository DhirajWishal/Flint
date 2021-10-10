// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ScreenSpaceReflectionPass.hpp"
#include "TestBase/DefaultProcessingPipeline.hpp"
#include "Engine/ProcessingPipeline/ProcessingPipeline.hpp"
#include "Engine/ShaderCompiler.hpp"
#include "GraphicsCore/ResourcePackage.hpp"

namespace Flint
{
	ScreenSpaceReflectionPass::ScreenSpaceReflectionPass(ProcessingPipeline* pProcessingPipeline, OffScreenPass* pOffScreenPass)
		: ProcessingNode(pProcessingPipeline), pOffScreenPass(pOffScreenPass)
	{
		const auto pDevice = GetDevice();
		std::shared_ptr<Shader> pShader = nullptr;

		// Check if we have to compile the source. If so compile.
		if (!std::filesystem::exists("Flint/Shaders/ScreenSpaceReflections.comp.fsc"))
		{
			ShaderCompiler compiler(std::filesystem::path("Shaders/SSR/ScreenSpaceReflections.comp"), ShaderCodeType::GLSL, ShaderType::Compute);
			pShader = compiler.CreateShader(pDevice);
			pShader->CreateCache("Flint/Shaders/ScreenSpaceReflections.comp.fsc");
		}
		else
			pShader = pDevice->CreateShader(ShaderType::Compute, std::filesystem::path("Flint/Shaders/ScreenSpaceReflections.comp.fsc"));

		//pShader = pDevice->CreateShader(ShaderType::Compute, std::filesystem::path("Shaders/SSR/comp.spv"));

		// Create the compute pipeline.
		pComputePipeline = pDevice->CreateComputePipeline("SSRPipeline", pShader);
		pLensProjection = pDevice->CreateBuffer(BufferType::Uniform, sizeof(LensProjection));

		// Create the anti aliasing image.
		auto pOffScreenImage = pOffScreenPass->GetImageAttachments()[0].pImage;
		pOutputImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Storage, pOffScreenImage->GetExtent(), pOffScreenImage->GetFormat(), pOffScreenImage->GetLayerCount(), pOffScreenImage->GetMipLevels(), nullptr);

		pResourcePackages.reserve(GetBufferCount());
		for (UI32 i = 0; i < GetBufferCount(); i++)
		{
			const auto pResourcePackage = pComputePipeline->CreateResourcePackage(0);

			// Bind the resources.
			pOffScreenImage = pOffScreenPass->GetImageAttachments()[0].pImage;
			pResourcePackage->BindResource(0, pOffScreenImage, pOffScreenImage->CreateImageView(0, pOffScreenImage->GetLayerCount(), 0, pOffScreenImage->GetMipLevels(), ImageUsage::Graphics), pDevice->CreateImageSampler(ImageSamplerSpecification()));

			pOffScreenImage = pOffScreenPass->GetImageAttachments()[1].pImage;
			pResourcePackage->BindResource(1, pOffScreenImage, pOffScreenImage->CreateImageView(0, pOffScreenImage->GetLayerCount(), 0, pOffScreenImage->GetMipLevels(), ImageUsage::Graphics), pDevice->CreateImageSampler(ImageSamplerSpecification()));

			pOffScreenImage = pOffScreenPass->GetImageAttachments()[2].pImage;
			pResourcePackage->BindResource(2, pOffScreenImage, pOffScreenImage->CreateImageView(0, pOffScreenImage->GetLayerCount(), 0, pOffScreenImage->GetMipLevels(), ImageUsage::Depth), pDevice->CreateImageSampler(ImageSamplerSpecification()));

			pResourcePackage->BindResource(3, pOutputImage, pOutputImage->CreateImageView(0, pOutputImage->GetLayerCount(), 0, pOutputImage->GetMipLevels(), ImageUsage::Storage), pDevice->CreateImageSampler(ImageSamplerSpecification()), ImageUsage::Storage);

			pResourcePackage->BindResource(4, pLensProjection);

			pResourcePackages.emplace_back(pResourcePackage);
		}
	}

	void ScreenSpaceReflectionPass::Process(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 frameIndex, const UI32 imageIndex)
	{
		pLensData = static_cast<LensProjection*>(pLensProjection->MapMemory(pLensProjection->GetSize()));
		pLensData->mMatrix = pProcessingPipeline->StaticCast<DefaultProcessingPipeline>().GetCamera().GetMatrix().mProjectionMatrix;
		pLensProjection->UnmapMemory();

		pCommandBuffer->BindRenderTarget(pProcessingPipeline->GetScreenBoundRenderTarget().get());
		pCommandBuffer->UnbindRenderTarget();

		pCommandBuffer->BindComputePipeline(pComputePipeline.get());
		pCommandBuffer->BindResourcePackage(pComputePipeline.get(), pResourcePackages[frameIndex].get());

		const auto workGroup = FBox3D(pOutputImage->GetExtent().mWidth / 32, pOutputImage->GetExtent().mHeight / 32, 1);
		pCommandBuffer->IssueComputeCall(workGroup);

		// Get the color image of the processing pipeline.
		const auto pColorImage = pProcessingPipeline->GetColorBuffer();

		// If the color image is present, copy the color image from the off screen pass to it. Else copy the image to the swap chain.
		if (pColorImage)
			pCommandBuffer->CopyImage(pOutputImage.get(), 0, pColorImage.get(), 0);
		else
			pCommandBuffer->CopyToSwapChainImage(pOutputImage.get(), 0, pProcessingPipeline->GetScreenBoundRenderTarget()->GetSwapChain().get(), imageIndex, 0);
	}
}
