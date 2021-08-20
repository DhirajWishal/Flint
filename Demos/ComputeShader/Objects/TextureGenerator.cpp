// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "TextureGenerator.hpp"
#include "Components/ShaderCompiler.hpp"
#include "Flint/Instance.hpp"

TextureGenerator::TextureGenerator(SceneState* pSceneState)
	: pSceneState(pSceneState)
{
	auto pInstance = Flint::CreateInstance(true);
	auto pDevice = pInstance->CreateDevice(Flint::DeviceFlags::COMPUTE_COMPATIBLE);

	//CompileAllShaders(std::filesystem::path(__FILE__).parent_path().parent_path().parent_path().parent_path());

	auto pTexture = pDevice->CreateImage(Flint::ImageType::DIMENSIONS_2, Flint::ImageUsage::STORAGE, Flint::FBox3D(1024, 1024, 1), Flint::PixelFormat::R8G8B8A8_UNORMAL, 1, 1, nullptr);
	auto pShader = pDevice->CreateShader(Flint::ShaderType::COMPUTE, std::filesystem::path("Flint\\Shaders\\TextureGenerator\\TextureGen.comp.spv"));
	auto pTextureSampler = pDevice->CreateImageSampler(Flint::ImageSamplerSpecification());
	auto pComputePipeline = pDevice->CreateComputePipeline("TextGenerator", pShader);

	auto pResourceMap = pComputePipeline->CreateResourceMap();
	pResourceMap->SetResource("image", pTextureSampler, pTexture);
	pComputePipeline->AddInstance(pResourceMap, nullptr, Flint::FBox3D(1024.0f / 16, 1024.0f / 16, 1));
	pComputePipeline->Dispatch();
}

TextureGenerator::~TextureGenerator()
{
}
