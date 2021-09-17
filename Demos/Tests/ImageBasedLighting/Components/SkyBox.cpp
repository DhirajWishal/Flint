// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SkyBox.hpp"

#include "TestBase/GraphicsScene.hpp"
#include "Engine/ShaderCompiler.hpp"
#include "Engine/ImageLoader.hpp"
#include "Engine/Tools/CubeMapGenerator.hpp"

#include <array>

namespace Flint
{
	void SkyBox::Initialize(Application* pApplication)
	{
		this->pApplication = pApplication;
		pDynamicStates = std::make_shared<DynamicStateContainer>();

		CreatePipeline();
		LoadAsset();
		LoadTextures();

		pUniformBuffer = pApplication->GetDevice()->CreateBuffer(BufferType::Uniform, sizeof(ModelViewProjection));

		pPackager = pPipeline->CreateResourcePackagers()[0];
		pPackage = pPackager->CreatePackage();
		pPackage->BindResource(0, pUniformBuffer);
		pPackage->BindResource(1, pImageSampler, pImage);
	}

	void SkyBox::Update(UI64 delta, Camera* pCamera)
	{
		auto mat = pCamera->GetMatrix();
		mMatrix.mViewMatrix = mat.mViewMatrix;
		mMatrix.mProjectionMatrix = mat.mProjectionMatrix;

		ImGui::Begin("Level Of Detail");
		ImGui::SliderFloat("LOD Bias", &mMatrix.mLODBias, 0, static_cast<float>(pImage->GetMipLevels()));
		ImGui::End();

		CopyToBuffer(pUniformBuffer, mMatrix);
	}

	void SkyBox::Draw(const std::shared_ptr<CommandBuffer>& pCommandBuffer)
	{
		pCommandBuffer->BindGraphicsPipeline(pPipeline);
		pCommandBuffer->BindGeometryStore(mAsset.GetGeometryStore());
		pCommandBuffer->BindDynamicStates(pPipeline, pDynamicStates);
		pCommandBuffer->BindResourcePackages(pPipeline, { pPackage });

		for (const auto wireFrame : mAsset.GetWireFrames())
			pCommandBuffer->IssueDrawCall(wireFrame);
	}

	void SkyBox::Terminate()
	{
		pUniformBuffer->Terminate();
		pImage->Terminate();
		pImageSampler->Terminate();

		pPackager->Terminate();

		mAsset.Clear();
	}

	void SkyBox::CreatePipeline()
	{
		Flint::GraphicsPipelineSpecification specification = {};
		specification.mRasterizationSamples = pApplication->pDevice->GetSupportedMultiSampleCount();
		specification.mFrontFace = Flint::FrontFace::Clockwise;
		specification.mDynamicStateFlags = Flint::DynamicStateFlags::ViewPort | Flint::DynamicStateFlags::Scissor;

		ShaderCompiler vertexShaderCompiler(std::filesystem::path("E:\\Flint\\Demos\\Tests\\ImageBasedLighting\\Shaders\\SkyBox\\SkyBox.vert"), ShaderCodeType::GLSL, ShaderType::Vertex);
		ShaderCompiler fragmentShaderCompiler(std::filesystem::path("E:\\Flint\\Demos\\Tests\\ImageBasedLighting\\Shaders\\SkyBox\\SkyBox.frag"), ShaderCodeType::GLSL, ShaderType::Fragment);

		auto pDevice = pApplication->GetDevice();
		auto pVertexShader = pDevice->CreateShader(ShaderType::Vertex, vertexShaderCompiler.GetShaderCode());
		pPipeline = pApplication->GetGraphicsScene("Default")->CreateGraphicsPipeline("SkyBox",
			pVertexShader,
			pDevice->CreateShader(ShaderType::Fragment, fragmentShaderCompiler.GetShaderCode()),
			specification);

		pApplication->CreateGeometryStore("SkyBox", pVertexShader->GetInputAttributes(), sizeof(UI32));

		auto windowExtent = pApplication->GetGraphicsScene("Default")->GetDisplay()->GetExtent();
		pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
		pDynamicStates->SetScissor(windowExtent, { 0, 0 });
	}

	void SkyBox::LoadAsset()
	{
		mAsset = Asset("E:\\Flint\\Assets\\Packages\\Skybox\\Skybox\\SkySphere.obj", pApplication->GetGeometryStore("SkyBox"), Defaults::CreateSkyBoxVertexDescriptor());
	}

	void SkyBox::LoadTextures()
	{
		//std::vector<std::filesystem::path> images = {
		//	"E:\\Flint\\Assets\\Textures\\SkyBox\\right.jpg",
		//	"E:\\Flint\\Assets\\Textures\\SkyBox\\left.jpg",
		//	"E:\\Flint\\Assets\\Textures\\SkyBox\\top.jpg",
		//	"E:\\Flint\\Assets\\Textures\\SkyBox\\bottom.jpg",
		//	"E:\\Flint\\Assets\\Textures\\SkyBox\\front.jpg",
		//	"E:\\Flint\\Assets\\Textures\\SkyBox\\back.jpg"
		//};
		//
		//ImageLoader loader(images);
		//pImage = pApplication->GetDevice()->CreateImage(ImageType::CubeMap, ImageUsage::Graphics, loader.GetExtent(), loader.GetPixelFormat(), 6, 1, loader.GetPixelData());

		CubeMapGenerator generator(pApplication->GetDevice(), "C:\\Users\\RLG Evo\\Downloads\\oberer_kuhberg_4k.hdr", 0);
		pImage = generator.GetImage();

		Flint::ImageSamplerSpecification samplerSpecification = {};
		samplerSpecification.mAddressModeU = Flint::AddressMode::ClampToEdge;
		samplerSpecification.mAddressModeV = Flint::AddressMode::ClampToEdge;
		samplerSpecification.mAddressModeW = Flint::AddressMode::ClampToEdge;
		samplerSpecification.mMaxLevelOfDetail = static_cast<float>(pImage->GetMipLevels());
		pImageSampler = pApplication->GetDevice()->CreateImageSampler(samplerSpecification);
	}
}