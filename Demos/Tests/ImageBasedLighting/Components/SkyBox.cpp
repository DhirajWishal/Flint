// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SkyBox.hpp"

#include "TestBase/GraphicsScene.hpp"
#include "Engine/ShaderCompiler.hpp"
#include "Engine/ImageLoader.hpp"
#include "Engine/Tools/CubeMapGenerator.hpp"

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

		pPackage = pPipeline->CreateResourcePackage(0);
		pPackage->BindResource(0, pUniformBuffer);
		pPackage->BindResource(1, pImageSampler, pImage);
	}

	void SkyBox::Update(uint64 delta, Camera* pCamera)
	{
		auto mat = pCamera->GetMatrix();
		mMatrix.mViewMatrix = mat.mViewMatrix;
		mMatrix.mProjectionMatrix = mat.mProjectionMatrix;

		ImGui::Begin("Level Of Detail");
		ImGui::SliderFloat("LOD Bias", &mMatrix.mLODBias, 0, static_cast<float>(pImage->GetMipLevels()));
		ImGui::SliderFloat("Exposure", &mMatrix.mExposure, 0, 10.0f);
		ImGui::SliderFloat("Gamma", &mMatrix.mGamma, 0, 10.0f);
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
		pPipeline->Terminate();

		mAsset.Clear();
	}

	void SkyBox::CreatePipeline()
	{
		auto pDevice = pApplication->GetDevice();
		std::shared_ptr<Shader> pVertexShader = nullptr;
		std::shared_ptr<Shader> pFragmentShader = nullptr;

		if (!std::filesystem::exists("Flint\\Shaders\\SkyBox.vert.fsc"))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path("E:\\Flint\\Demos\\Tests\\ImageBasedLighting\\Shaders\\SkyBox\\SkyBox.vert"), ShaderCodeType::GLSL, ShaderType::Vertex);
			pVertexShader = shaderCompiler.CreateShader(pDevice);
			pVertexShader->CreateCache("Flint\\Shaders\\Skybox.vert.fsc");
		}
		else
			pVertexShader = pDevice->CreateShader(ShaderType::Vertex, std::filesystem::path("Flint\\Shaders\\SkyBox.vert.fsc"));

		if (!std::filesystem::exists("Flint\\Shaders\\SkyBox.frag.fsc"))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path("E:\\Flint\\Demos\\Tests\\ImageBasedLighting\\Shaders\\SkyBox\\SkyBox.frag"), ShaderCodeType::GLSL, ShaderType::Fragment);
			pFragmentShader = shaderCompiler.CreateShader(pDevice);
			pFragmentShader->CreateCache("Flint\\Shaders\\Skybox.frag.fsc");
		}
		else
			pFragmentShader = pDevice->CreateShader(ShaderType::Fragment, std::filesystem::path("Flint\\Shaders\\SkyBox.frag.fsc"));

		Flint::GraphicsPipelineSpecification specification = {};
		specification.mRasterizationSamples = pApplication->pDevice->GetSupportedMultiSampleCount();
		specification.mFrontFace = Flint::FrontFace::Clockwise;
		specification.mDynamicStateFlags = Flint::DynamicStateFlags::ViewPort | Flint::DynamicStateFlags::Scissor;

		specification.mVertexInputAttributeMap[0] = pVertexShader->GetInputAttributes();
		pPipeline = pApplication->GetGraphicsScene("Default")->CreateGraphicsPipeline("SkyBox", pVertexShader, pFragmentShader, specification);

		pApplication->CreateGeometryStore("SkyBox", pVertexShader->GetInputAttributes(), sizeof(uint32));

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
		CubeMapGenerator generator(pApplication->GetDevice(), "C:\\Users\\RLG Evo\\Downloads\\piazza_martin_lutero_4k.hdr", 0);
		pImage = generator.GetImage();

		Flint::ImageSamplerSpecification samplerSpecification = {};
		samplerSpecification.mAddressModeU = Flint::AddressMode::ClampToEdge;
		samplerSpecification.mAddressModeV = Flint::AddressMode::ClampToEdge;
		samplerSpecification.mAddressModeW = Flint::AddressMode::ClampToEdge;
		samplerSpecification.mMaxLevelOfDetail = static_cast<float>(pImage->GetMipLevels());
		pImageSampler = pApplication->GetDevice()->CreateImageSampler(samplerSpecification);
	}
}