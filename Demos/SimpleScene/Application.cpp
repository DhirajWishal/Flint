// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.h"

static Flint::Vector3 Forward = Flint::Vector3(1.0f, 0.0f, 0.0f);
static Flint::Vector3 WorldUp = Flint::Vector3(0.0f, 1.0f, 0.0f);
const Flint::Vector2 WindowExtent = Flint::Vector2(1280.0f, 720.0f);

float Application::Dot(Flint::Vector3 lhs, Flint::Vector3 rhs)
{
	Flint::Vector3 _tmp(lhs * rhs);
	return _tmp.x + _tmp.y + _tmp.z;
}

Flint::Vector3 Application::Cross(Flint::Vector3 lhs, Flint::Vector3 rhs)
{
	return (Flint::Vector3(lhs.y, lhs.z, lhs.x) * Flint::Vector3(rhs.z, rhs.x, rhs.y)) -
		(Flint::Vector3(rhs.y, rhs.z, rhs.x) * Flint::Vector3(lhs.z, lhs.x, lhs.y));
}

Flint::Vector3 Application::Normalize(Flint::Vector3 rhs)
{
	return rhs * (1.0f / std::sqrt(Dot(rhs, rhs)));
}

Flint::Matrix44 Application::LookAt(Flint::Vector3 const eye, Flint::Vector3 const center, Flint::Vector3 const up)
{
	auto const f = Normalize(center - eye);
	auto const s = Normalize(Cross(f, up));
	auto const u = Cross(s, f);

	Flint::Matrix44 newMatrix(1.0f);
	newMatrix[0][0] = s.x;
	newMatrix[1][0] = s.y;
	newMatrix[2][0] = s.z;
	newMatrix[3][0] = -Dot(s, eye);

	newMatrix[0][1] = u.x;
	newMatrix[1][1] = u.y;
	newMatrix[2][1] = u.z;
	newMatrix[3][1] = -Dot(u, eye);

	newMatrix[0][2] = -f.x;
	newMatrix[1][2] = -f.y;
	newMatrix[2][2] = -f.z;
	newMatrix[3][2] = Dot(f, eye);

	return newMatrix;
}

Flint::Matrix44 Application::Perspective(float FOV, float aspect, float near, float far, bool flipYAxis)
{
	float const halfTan = tan(FOV / 2.0f);

	Flint::Matrix44 newMatrix(0.0f);
	newMatrix[0][0] = 1.0f / (aspect * halfTan);
	newMatrix[1][1] = 1.0f / (halfTan);
	newMatrix[2][2] = far / (near - far);
	newMatrix[2][3] = -1.0f;
	newMatrix[3][2] = -(far * near) / (far - near);

	if (flipYAxis)
		newMatrix[1][1] *= -1.0f;

	return newMatrix;
}

Application::Application()
{
	// Initialize the instance.
#ifdef FLINT_DEBUG
	mInstance.Initialize(true);

#else
	mInstance.Initialize(false);

#endif // FLINT_DEBUG

	// Initialize the display.
	mDisplay.Initialize(mInstance, WindowExtent, "Flint- Sample Scene");

	// Initialize the device.
	mDevice.Initialize(mInstance);

	// Create the required render target.
	mRenderTarget = mDevice.CreateScreenBoundRenderTarget(mDisplay, std::numeric_limits<UI64>::max());

	// Setup shaders.
	SetupShaders();

	// Initialize the pipeline.
	mPipeline = mRenderTarget.CreateGraphicsPipeline({ mShaders[0], mShaders[1] }, GetGraphicsPipelineSpec());

	// Load wireframe information.
	mWireFrame.LoadFromCache("testCache.wfc", mDevice);

	// Add the wire frame to the draw queue.
	mDrawIndex = mRenderTarget.AddWireFrameToStaticDrawQueue(mWireFrame);
}

Application::~Application()
{
	// Remove the wire frame from the draw queue.
	mRenderTarget.RemoveWireFrameFromStaticDrawQueue(mDrawIndex);

	// Clear the wireframe.
	mWireFrame.Clear();

	// Destroy the pipeline object.
	mPipeline.Terminate();

	// Terminate the render target.
	mRenderTarget.Terminate();

	// Terminate the display.
	mDisplay.Terminate();

	// Terminate the device.
	mDevice.Terminate();

	// Terminate the instance.
	mInstance.Terminate();
}

void Application::SetupShaders()
{
	mShaders[0].Initialize("E:\\Flint\\Assets\\Shaders\\TestShaders\\Default\\shader.vert.spv", Flint::ShaderLocation::VERTEX, Flint::ShaderCodeType::SPIR_V);
	mShaders[1].Initialize("E:\\Flint\\Assets\\Shaders\\TestShaders\\Default\\shader.frag.spv", Flint::ShaderLocation::FRAGMENT, Flint::ShaderCodeType::SPIR_V);
}

void Application::CreateSceneComponent()
{
}

void Application::PrepareToRender()
{
}

void Application::ProcessInputs()
{
}

void Application::OnUpdate()
{
}

void Application::DestroyRenderResources()
{
}

Flint::Backend::GraphicsPipelineSpecification Application::GetGraphicsPipelineSpec() const
{
	Flint::Backend::GraphicsPipelineSpecification spec = {};
	spec.mDynamicStateFlags = Flint::Backend::DynamicStateFlags::VIEWPORT | Flint::Backend::DynamicStateFlags::SCISSOR;
	spec.bEnableColorBlendLogic = false;
	spec.bEnableAlphaCoverage = false;
	spec.bEnableAlphaToOne = false;

	return spec;
}

/*
* Flint::AssetRegistry* pAssetRegistry = new Flint::AssetRegistry();
* auto pVertexShader = pAssetRegistry->CreateShader("vShader_1", GetAssetPath(), Flint::ShaderLocation::VERTEX, Flint::ShaderType::SPIRV);
* auto pFragmentShader = pAssetRegistry->CreateShader("fShader_1", GetAssetPath(), Flint::ShaderLocation::FRAGMENT, Flint::ShaderType::SPIRV);
* 
* auto shaderSamplers = pFragmentShader->GetShamplers();
* shaderSamplers["texSampler"] = GetDefaultSamplerSpec();
* pFragmentShader->SubmitSamplerSpecifications(shaderSamplers);
* 
* auto pWireFrame = pAssetRegistry->GetWireFrame("vWireFrame_1");
* auto pPipeline = pAssetRegistry->CreateGraphicsPipeline(pRenderTarget, pWireFrame, { pVertexShader, pFragmentShader }, GetPipelineSpec());
* 
* UI64 drawID = pRenderTarget->SubmitToDraw(pPipeline, pWireFrame, { mModelViewProj }, { mWrappingTexture });
* 
* pAssetRegistry->DestroyShader("vShader_1");
* pAssetRegistry->DestroyShader("fShader_1");
*/