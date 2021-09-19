// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Tools/CubeMapGenerator.hpp"
#include "Engine/ImageLoader.hpp"
#include "Engine/ShaderCompiler.hpp"

#include "GraphicsCore/OffScreenRenderTarget.hpp"
#include "GraphicsCore/ComputePipeline.hpp"
#include "GraphicsCore/ResourcePackager.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

namespace Flint
{
	CubeMapGenerator::CubeMapGenerator(const std::shared_ptr<Device>& pDevice, const std::filesystem::path& assetFile, UI32 mips)
	{
		if (assetFile.extension().string() != ".hdr")
			throw std::invalid_argument("Provided asset file is not supported!");

		ImageLoader loader(assetFile);
		auto pTexture = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Graphics, loader.GetExtent(), loader.GetPixelFormat(), 1, 1, loader.GetPixelData());

		UI32 length = loader.GetExtent().mWidth / 4;
		FBox3D extent = FBox3D(length, length, 1);

		if (mips == 0)
			mips = Image::GetBestMipLevels(FBox2D(length));

		pCubeMap = pDevice->CreateImage(ImageType::CubeMap, ImageUsage::Graphics | ImageUsage::Storage, extent, PixelFormat::R16G16B16A16_SFLOAT, 6, mips, nullptr);

		ShaderCompiler computeShaderCompiler(std::filesystem::path("E:\\Flint\\Code\\Engine\\Shaders\\CubeMapGen\\Shader.comp"), ShaderCodeType::GLSL, ShaderType::Compute);
		auto pPipeline = pDevice->CreateComputePipeline("CubeMapGen", computeShaderCompiler.CreateShader(pDevice));
		auto pPackage = pPipeline->CreateResourcePackage(0);

		Flint::ImageSamplerSpecification specification = {};
		pPackage->BindResource(0, pDevice->CreateImageSampler(specification), pTexture);

		specification.mAddressModeU = AddressMode::ClampToEdge;
		specification.mAddressModeV = AddressMode::ClampToEdge;
		specification.mAddressModeW = AddressMode::ClampToEdge;
		pPackage->BindResource(1, pDevice->CreateImageSampler(specification), pCubeMap, ImageUsage::Storage);

		auto pAllocator = pDevice->CreateCommandBufferAllocator(1);
		auto pCommandBuffers = pAllocator->CreateCommandBuffers();

		auto pCommandBuffer = pCommandBuffers.front();
		pCommandBuffer->BeginBufferRecording();

		pCommandBuffer->BindComputePipeline(pPipeline);
		pCommandBuffer->BindResourcePackages(pPipeline, { pPackage });
		pCommandBuffer->IssueComputeCall(FBox3D(length / 32, length / 32, 6));

		pCommandBuffer->EndBufferRecording();

		pDevice->SubmitComputeCommandBuffers({ pCommandBuffer });
		pCubeMap->GenerateMipMaps();
	}
}