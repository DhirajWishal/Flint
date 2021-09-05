// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VikingRoom.hpp"

#include "GraphicsCore/ScreenBoundRenderTarget.hpp"

#include "Engine/ShaderCompiler.hpp"
#include "Engine/AssetLoader.hpp"
#include "Engine/ImageLoader.hpp"

void VikingRoom::Initialize(const std::shared_ptr<Flint::Device>& pDevice, const std::shared_ptr<Flint::ScreenBoundRenderTarget>& pScreenBoundRenderTarget, const Camera* pCamera)
{
	this->pDevice = pDevice;
	this->pScreenBoundRenderTarget = pScreenBoundRenderTarget;
	this->pCamera = pCamera;

	pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();

	Flint::ShaderCompiler vertexShaderCompiler(std::filesystem::path("E:\\Flint\\Editor\\Shaders\\3D\\shader.vert"), Flint::ShaderCodeType::GLSL, Flint::ShaderType::Vertex);
	Flint::ShaderCompiler fragmentShaderCompiler(std::filesystem::path("E:\\Flint\\Editor\\Shaders\\3D\\shader.frag"), Flint::ShaderCodeType::GLSL, Flint::ShaderType::Fragment);

	pVertexShader = pDevice->CreateShader(Flint::ShaderType::Vertex, vertexShaderCompiler.GetShaderCode());
	pFragmentShader = pDevice->CreateShader(Flint::ShaderType::Fragment, fragmentShaderCompiler.GetShaderCode());
	pUniformBuffer = pDevice->CreateBuffer(Flint::BufferType::Uniform, sizeof(UniformBufferObject));

	pGeometryStore = pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI32));

	Flint::AssetLoader assetLoader(pGeometryStore, "E:\\Flint\\Assets\\Packages\\VikingRoom\\VikingRoom\\untitled.obj", Flint::Defaults::CreateDefaultVertexDescriptor());
	mAsset = Flint::Asset(assetLoader.GetWireFrames(), pGeometryStore);

	Flint::ImageLoader imageLoader("E:\\Flint\\Assets\\Packages\\VikingRoom\\VikingRoom\\texture.png");
	pTexture = pDevice->CreateImage(Flint::ImageType::TwoDimension, Flint::ImageUsage::Graphics, imageLoader.GetExtent(), imageLoader.GetPixelFormat(), 1, 1, imageLoader.GetPixelData());
	pTextureSampler = pDevice->CreateImageSampler(Flint::ImageSamplerSpecification());

	CreatePipeline();

	pResourcePackage->BindResource(0, pUniformBuffer);
	pResourcePackage->BindResource(1, pTextureSampler, pTexture);

	const auto windowExtent = pScreenBoundRenderTarget->GetExtent();
	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
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

void VikingRoom::SubmitToCommandBuffer(const std::shared_ptr<Flint::CommandBuffer>& pCommandBuffer)
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

void VikingRoom::CreatePipeline()
{
	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pDevice->GetSupportedMultiSampleCount();
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::ViewPort | Flint::DynamicStateFlags::Scissor;
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
