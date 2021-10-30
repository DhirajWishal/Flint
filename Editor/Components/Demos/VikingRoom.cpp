// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VikingRoom.hpp"

#include "GraphicsCore/ScreenBoundRenderTarget.hpp"

#include "Engine/ShaderCompiler.hpp"
#include "Engine/AssetLoader.hpp"
#include "Engine/ImageLoader.hpp"

namespace Flint
{
	void VikingRoom::Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget, const Camera* pCamera)
	{
		this->pDevice = pDevice;
		this->pRenderTarget = pScreenBoundRenderTarget;
		this->pCamera = pCamera;

		pDynamicStates = std::make_shared<DynamicStateContainer>();

		ShaderCompiler vertexShaderCompiler(std::filesystem::path("E:\\Flint\\Editor\\Shaders\\3D\\shader.vert"), ShaderCodeType::GLSL, ShaderType::Vertex);
		ShaderCompiler fragmentShaderCompiler(std::filesystem::path("E:\\Flint\\Editor\\Shaders\\3D\\shader.frag"), ShaderCodeType::GLSL, ShaderType::Fragment);

		pVertexShader = pDevice->CreateShader(ShaderType::Vertex, vertexShaderCompiler.GetShaderCode());
		pFragmentShader = pDevice->CreateShader(ShaderType::Fragment, fragmentShaderCompiler.GetShaderCode());
		pUniformBuffer = pDevice->CreateBuffer(BufferType::Uniform, sizeof(UniformBufferObject));

		pGeometryStore = pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(uint32));

		AssetLoader assetLoader(pGeometryStore, "E:\\Flint\\Assets\\Packages\\VikingRoom\\VikingRoom\\untitled.obj", Defaults::CreateDefaultVertexDescriptor());
		mAsset = Asset(assetLoader.GetWireFrames(), pGeometryStore);

		ImageLoader imageLoader("E:\\Flint\\Assets\\Packages\\VikingRoom\\VikingRoom\\texture.png");
		pTexture = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Graphics, imageLoader.GetExtent(), imageLoader.GetPixelFormat(), 1, 1, imageLoader.GetPixelData());
		pTextureSampler = pDevice->CreateImageSampler(ImageSamplerSpecification());

		CreatePipeline(pScreenBoundRenderTarget);

		pResourcePackage->BindResource(0, pUniformBuffer);
		pResourcePackage->BindResource(1, pTextureSampler, pTexture);

		const auto windowExtent = pScreenBoundRenderTarget->GetExtent();
		pDynamicStates->SetViewPort(FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
		pDynamicStates->SetScissor(windowExtent, { 0, 0 });
	}

	void VikingRoom::Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget, const Camera* pCamera)
	{
		this->pDevice = pDevice;
		this->pRenderTarget = pOffScreenRenderTarget;
		this->pCamera = pCamera;

		pDynamicStates = std::make_shared<DynamicStateContainer>();

		ShaderCompiler vertexShaderCompiler(std::filesystem::path("E:\\Flint\\Editor\\Shaders\\3D\\shader.vert"), ShaderCodeType::GLSL, ShaderType::Vertex);
		ShaderCompiler fragmentShaderCompiler(std::filesystem::path("E:\\Flint\\Editor\\Shaders\\3D\\shader.frag"), ShaderCodeType::GLSL, ShaderType::Fragment);

		pVertexShader = pDevice->CreateShader(ShaderType::Vertex, vertexShaderCompiler.GetShaderCode());
		pFragmentShader = pDevice->CreateShader(ShaderType::Fragment, fragmentShaderCompiler.GetShaderCode());
		pUniformBuffer = pDevice->CreateBuffer(BufferType::Uniform, sizeof(UniformBufferObject));

		pGeometryStore = pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(uint32));

		AssetLoader assetLoader(pGeometryStore, "E:\\Flint\\Assets\\Packages\\VikingRoom\\VikingRoom\\untitled.obj", Defaults::CreateDefaultVertexDescriptor());
		mAsset = Asset(assetLoader.GetWireFrames(), pGeometryStore);

		ImageLoader imageLoader("E:\\Flint\\Assets\\Packages\\VikingRoom\\VikingRoom\\texture.png");
		pTexture = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Graphics, imageLoader.GetExtent(), imageLoader.GetPixelFormat(), 1, 1, imageLoader.GetPixelData());
		pTextureSampler = pDevice->CreateImageSampler(ImageSamplerSpecification());

		CreatePipeline(pOffScreenRenderTarget);

		pResourcePackage->BindResource(0, pUniformBuffer);
		pResourcePackage->BindResource(1, pTextureSampler, pTexture);

		const auto windowExtent = pOffScreenRenderTarget->GetExtent();
		pDynamicStates->SetViewPort(FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
		pDynamicStates->SetScissor(windowExtent, { 0, 0 });
	}

	void VikingRoom::Recreate()
	{
		//pPipeline->Recreate(pRenderTarget);

		const auto windowExtent = pRenderTarget->GetExtent();
		pDynamicStates->SetViewPort(FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
		pDynamicStates->SetScissor(windowExtent, { 0, 0 });
	}

	void VikingRoom::Terminate()
	{
		pVertexShader->Terminate();
		pFragmentShader->Terminate();

		pUniformBuffer->Terminate();
		pGeometryStore->Terminate();

		pTexture->Terminate();
		pTextureSampler->Terminate();

		pPipeline->Terminate();
		pResourcePackager->Terminate();
	}

	void VikingRoom::SubmitToCommandBuffer(const std::shared_ptr<CommandBuffer>& pCommandBuffer)
	{
		const auto cameraData = pCamera->GetMatrix();
		mUniformBufferObject.mViewMatrix = cameraData.mViewMatrix;
		mUniformBufferObject.mProjectionMatrix = cameraData.mProjectionMatrix;

		SubmitToUniformBuffer(pUniformBuffer, mUniformBufferObject);

		pCommandBuffer->BindGeometryStore(pGeometryStore);
		pCommandBuffer->BindGraphicsPipeline(pPipeline);
		pCommandBuffer->BindDynamicStates(pPipeline, pDynamicStates);
		pCommandBuffer->BindResourcePackages(pPipeline, { pResourcePackage });

		for (const auto wireFrame : mAsset.GetWireFrames())
			pCommandBuffer->IssueDrawCall(wireFrame);
	}

	void VikingRoom::CreatePipeline(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget)
	{
		GraphicsPipelineSpecification specification = {};
		//specification.mRasterizationSamples = MultiSampleCount::One;
		specification.mRasterizationSamples = pDevice->GetSupportedMultiSampleCount();
		specification.mDynamicStateFlags = DynamicStateFlags::ViewPort | DynamicStateFlags::Scissor;
		specification.bEnableDepthTest = true;
		specification.bEnableDepthWrite = true;
		specification.mColorBlendConstants[0] = 0.0f;
		specification.mColorBlendConstants[1] = 0.0f;
		specification.mColorBlendConstants[2] = 0.0f;
		specification.mColorBlendConstants[3] = 0.0f;

		pPipeline = pDevice->CreateGraphicsPipeline("VikingRoom", pScreenBoundRenderTarget, pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
		pResourcePackager = pPipeline->CreateResourcePackagers()[0];
		pResourcePackage = pResourcePackager->CreatePackage();
	}

	void VikingRoom::CreatePipeline(const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget)
	{
		GraphicsPipelineSpecification specification = {};
		specification.mRasterizationSamples = MultiSampleCount::One;
		//specification.mRasterizationSamples = pDevice->GetSupportedMultiSampleCount();
		specification.mDynamicStateFlags = DynamicStateFlags::ViewPort | DynamicStateFlags::Scissor;
		specification.bEnableDepthTest = true;
		specification.bEnableDepthWrite = true;
		specification.mColorBlendConstants[0] = 0.0f;
		specification.mColorBlendConstants[1] = 0.0f;
		specification.mColorBlendConstants[2] = 0.0f;
		specification.mColorBlendConstants[3] = 0.0f;

		pPipeline = pDevice->CreateGraphicsPipeline("VikingRoom", pOffScreenRenderTarget, pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
		pResourcePackager = pPipeline->CreateResourcePackagers()[0];
		pResourcePackage = pResourcePackager->CreatePackage();
	}
}
