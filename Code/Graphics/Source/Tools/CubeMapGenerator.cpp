// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Tools/CubeMapGenerator.hpp"
#include "Graphics/ImageLoader.hpp"
#include "Graphics/ShaderCompiler.hpp"

#include "GraphicsCore/OffScreenRenderTarget.hpp"
#include "GraphicsCore/ComputePipeline.hpp"
#include "GraphicsCore/ResourcePackager.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

namespace Flint
{
	CubeMapGenerator::CubeMapGenerator(const std::shared_ptr<Device>& pDevice, const std::filesystem::path& assetFile, uint32 mips)
	{
		if (assetFile.extension().string() != ".hdr")
			throw std::invalid_argument("Provided asset file is not supported!");

		ImageLoader loader(assetFile);
		auto pTexture = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Graphics, loader.GetExtent(), loader.GetPixelFormat(), 1, 1, loader.GetPixelData());

		uint32 length = loader.GetExtent().mWidth / 4;
		FBox3D extent = FBox3D(length, length, 1);

		if (mips == 0)
			mips = Image::GetBestMipLevels(extent);

		pCubeMap = pDevice->CreateImage(ImageType::CubeMap, ImageUsage::Graphics | ImageUsage::Storage, extent, PixelFormat::R16G16B16A16_SFLOAT, 6, mips, nullptr);

		std::shared_ptr<Shader> pShader = nullptr;
		if (!std::filesystem::exists(NormalizePath("Flint\\Shaders\\CubeMapGen.fsc")))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path(NormalizePath("E:\\Flint\\Code\\Engine\\Shaders\\CubeMapGen\\Shader.comp")), ShaderCodeType::GLSL, ShaderType::Compute);
			pShader = shaderCompiler.CreateShader(pDevice);
			pShader->CreateCache(NormalizePath("Flint\\Shaders\\CubeMapGen.fsc"));
		}
		else
			pShader = pDevice->CreateShader(ShaderType::Compute, std::filesystem::path(NormalizePath("Flint\\Shaders\\CubeMapGen.fsc")));

		auto pPipeline = pDevice->CreateComputePipeline("CubeMapGen", pShader);
		auto pPackage = pPipeline->CreateResourcePackage(0);

		Flint::ImageSamplerSpecification specification = {};
		pPackage->BindResource(0, pTexture, pTexture->CreateImageView(0, pTexture->GetLayerCount(), 0, pTexture->GetMipLevels(), ImageUsage::Graphics), pDevice->CreateImageSampler(specification));

		specification.mAddressModeU = AddressMode::ClampToEdge;
		specification.mAddressModeV = AddressMode::ClampToEdge;
		specification.mAddressModeW = AddressMode::ClampToEdge;
		pPackage->BindResource(1, pCubeMap, pCubeMap->CreateImageView(0, pCubeMap->GetLayerCount(), 0, pCubeMap->GetMipLevels(), ImageUsage::Graphics), pDevice->CreateImageSampler(specification), ImageUsage::Storage);

		auto pAllocator = pDevice->CreateCommandBufferAllocator(1);
		auto pCommandBuffers = pAllocator->CreateCommandBuffers();

		auto pCommandBuffer = pCommandBuffers.front();
		pCommandBuffer->BeginBufferRecording();

		pCommandBuffer->BindComputePipeline(pPipeline.get());
		pCommandBuffer->BindResourcePackage(pPipeline.get(), pPackage.get());
		pCommandBuffer->IssueComputeCall(FBox3D(length / 32, length / 32, 6));

		pCommandBuffer->EndBufferRecording();

		pDevice->SubmitComputeCommandBuffer(pCommandBuffer.get());
		pCubeMap->GenerateMipMaps();
	}
}