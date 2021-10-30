// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ScreenSpaceReflectionPass.hpp"
#include "TestBase/DefaultProcessingPipeline.hpp"
#include "Graphics/ProcessingPipeline/ProcessingPipeline.hpp"
#include "Graphics/ShaderCompiler.hpp"
#include "GraphicsCore/ResourcePackage.hpp"

#include <optick.h>

namespace Flint
{
	ScreenSpaceReflectionPass::ScreenSpaceReflectionPass(ProcessingPipeline* pProcessingPipeline, OffScreenPass* pOffScreenPass)
		: ProcessingNode(pProcessingPipeline), pOffScreenPass(pOffScreenPass)
	{
		OPTICK_EVENT();

		const auto pDevice = GetDevice();
		std::shared_ptr<Shader> pShader = nullptr;

		// Check if we have to compile the source. If so compile.
		if (!std::filesystem::exists(NormalizePath("Flint/Shaders/ScreenSpaceReflections.comp.fsc")))
		{
			ShaderCompiler compiler(std::filesystem::path(NormalizePath("Shaders/SSR/ScreenSpaceReflections.comp")), ShaderCodeType::GLSL, ShaderType::Compute);
			pShader = compiler.CreateShader(pDevice);
			pShader->CreateCache(NormalizePath("Flint/Shaders/ScreenSpaceReflections.comp.fsc"));
		}
		else
			pShader = pDevice->CreateShader(ShaderType::Compute, std::filesystem::path(NormalizePath("Flint/Shaders/ScreenSpaceReflections.comp.fsc")));

		// Create the compute pipeline.
		pComputePipeline = pDevice->CreateComputePipeline("SSRPipeline", pShader);
		pLensProjection = pDevice->CreateBuffer(BufferType::Uniform, sizeof(LensProjection));

		// Create the anti aliasing image.
		auto pOffScreenImage = pOffScreenPass->GetImageAttachments()[0].pImage;
		pOutputImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Storage, pOffScreenImage->GetExtent(), pOffScreenImage->GetFormat(), pOffScreenImage->GetLayerCount(), pOffScreenImage->GetMipLevels(), nullptr);

		pResourcePackages.reserve(GetBufferCount());
		for (uint32 i = 0; i < GetBufferCount(); i++)
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

		pLensData = static_cast<LensProjection*>(pLensProjection->MapMemory(pLensProjection->GetSize()));
	}

	ScreenSpaceReflectionPass::~ScreenSpaceReflectionPass()
	{
		pLensProjection->UnmapMemory();
	}

	void ScreenSpaceReflectionPass::Process(ProcessingNode* pPreviousNode, const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 frameIndex, const uint32 imageIndex)
	{
		OPTICK_EVENT();

		pLensData->mMatrix = pProcessingPipeline->StaticCast<DefaultProcessingPipeline>().GetCamera().GetMatrix().mProjectionMatrix;

		ImGui::Begin("Screen Space Reflections");
		ImGui::SliderFloat("Contribution", &pLensData->mContribution, 0.1f, 1.0f);
		ImGui::SliderFloat("Min Ray Step", &pLensData->mMinRayStep, 0.1f, 40.0f);
		ImGui::Checkbox("Enable/ Disable", &bIsEnabled);
		ImGui::End();

		// Compute if enabled.
		if (bIsEnabled)
		{
			pCommandBuffer->BindComputePipeline(pComputePipeline.get());
			pCommandBuffer->BindResourcePackage(pComputePipeline.get(), pResourcePackages[frameIndex].get());
			pCommandBuffer->IssueComputeCall(FBox3D(pOutputImage->GetExtent().mWidth / 32, pOutputImage->GetExtent().mHeight / 32, 1));
		}

		// Just copy if disabled.
		else
			pCommandBuffer->CopyImage(pOffScreenPass->GetColorImage().get(), 0, pOutputImage.get(), 0);
	}
}
