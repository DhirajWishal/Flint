// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Tools/CubeMapGenerator.hpp"
#include "Graphics/Tools/ImageLoader.hpp"
#include "Graphics/Tools/ShaderCompiler.hpp"

#include "GraphicsCore/OffScreenRenderTarget.hpp"
#include "GraphicsCore/ComputePipeline.hpp"
#include "GraphicsCore/ResourcePackager.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

namespace Flint
{
	CubeMapGenerator::CubeMapGenerator(Device* m_pDevice, const std::filesystem::path& assetFile, uint32_t mips)
	{
		if (assetFile.extension().string() != ".hdr")
			throw std::invalid_argument("Provided asset file is not supported!");

		ImageLoader loader(assetFile);
		auto pTexture = m_pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Graphics, loader.GetExtent(), loader.GetPixelFormat(), 1, 1, loader.GetPixelData());
		auto pTextureView = pTexture->CreateImageView(0, pTexture->GetLayerCount(), 0, pTexture->GetMipLevels(), ImageUsage::Graphics);

		uint32_t length = loader.GetExtent().m_Width / 4;
		FBox3D extent = FBox3D(length, length, 1);

		if (mips == 0)
			mips = Image::GetBestMipLevels(extent);

		pCubeMap = m_pDevice->CreateImage(ImageType::CubeMap, ImageUsage::Graphics | ImageUsage::Storage, extent, PixelFormat::R16G16B16A16_SFLOAT, 6, mips, nullptr);
		auto pCubeMapView = pCubeMap->CreateImageView(0, pCubeMap->GetLayerCount(), 0, pCubeMap->GetMipLevels(), ImageUsage::Graphics);

		std::unique_ptr<Shader> pShader = nullptr;
		if (!std::filesystem::exists(NormalizePath("Flint\\Shaders\\CubeMapGen.fsc")))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path(NormalizePath("E:\\Flint\\Code\\Engine\\Shaders\\CubeMapGen\\Shader.comp")), ShaderCodeType::GLSL, ShaderType::Compute);
			pShader = shaderCompiler.CreateShader(m_pDevice);
			pShader->CreateCache(NormalizePath("Flint\\Shaders\\CubeMapGen.fsc"));
		}
		else
			pShader = m_pDevice->CreateShader(ShaderType::Compute, std::filesystem::path(NormalizePath("Flint\\Shaders\\CubeMapGen.fsc")));

		auto pPipeline = m_pDevice->CreateComputePipeline("CubeMapGen", std::move(pShader));
		auto pPackage = pPipeline->CreateResourcePackage(0);

		Flint::ImageSamplerSpecification specification = {};
		auto pSampler = m_pDevice->CreateImageSampler(specification);

		pPackage->BindResource(0, pTexture.get(), pTextureView.get(), pSampler.get());

		specification.m_AddressModeU = AddressMode::ClampToEdge;
		specification.m_AddressModeV = AddressMode::ClampToEdge;
		specification.m_AddressModeW = AddressMode::ClampToEdge;
		pPackage->BindResource(1, pCubeMap.get(), pCubeMapView.get(), pSampler.get(), ImageUsage::Storage);

		auto pAllocator = m_pDevice->CreateCommandBufferAllocator(1);
		auto pCommandBuffers = pAllocator->CreateCommandBuffers();

		auto pCommandBuffer = pCommandBuffers.front();
		pCommandBuffer->BeginBufferRecording();

		pCommandBuffer->BindComputePipeline(pPipeline.get());
		pCommandBuffer->BindResourcePackage(pPipeline.get(), pPackage.get());
		pCommandBuffer->IssueComputeCall(FBox3D(length / 32, length / 32, 6));

		pCommandBuffer->EndBufferRecording();

		m_pDevice->SubmitComputeCommandBuffer(pCommandBuffer.get());
		pCubeMap->GenerateMipMaps();
	}
}