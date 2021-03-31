// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.h"

static Flint::Vector3 Forward = Flint::Vector3(1.0f, 0.0f, 0.0f);
static Flint::Vector3 WorldUp = Flint::Vector3(0.0f, 1.0f, 0.0f);

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
	// Initialize the engine.
#ifdef FLINT_DEBUG
	mEngine.Initialize(Flint::BackendAPI::VULKAN, true, mWidth, mHeight, "Flint: Simple Scene");

#else
	mEngine.Initialize(Flint::BackendAPI::VULKAN, false, mWidth, mHeight, "Flint: Simple Scene");

#endif // FLINT_DEBUG


	// Get the input center.
	pInputCenter = mEngine.GetDisplay()->GetInputCenter();

	// Initialize the render target.
	mEngine.CreateRenderTarget();

	//{
	//	Flint::ShaderCode shaderCode = {};
	//	shaderCode.LoadFromFile("E:\\Flint\\Assets\\Objects\\Moon\\shader.vert", Flint::ShaderLocation::VERTEX, Flint::ShaderCodeType::GLSL);
	//	shaderCode.Transpile(Flint::ShaderCodeType::SPIR_V);
	//}

	// Create the scene component.
	CreateSceneComponent();

	// Prepare resources to render.
	PrepareToRender();

	// Run the main loop until the window is open.
	while (pInputCenter->IsWindowOpen)
	{
		mEngine.BeginUpdate();

		// Process inputs.
		ProcessInputs();

		// Perform on update tasks.
		OnUpdate();

		mEngine.EndUpdate();
	}
}

Application::~Application()
{
	// Destroy the used render resources.
	DestroyRenderResources();

	// Terminate the engine.
	mEngine.Terminate();
}

std::vector<Flint::ShaderDigest> Application::CreateShaderDigests() const
{
	Flint::ShaderCode shaders[2] = {};
	shaders[0].LoadFromFile("E:\\Flint\\Assets\\Shaders\\TestShaders\\Default\\shader.vert.spv", Flint::ShaderLocation::VERTEX, Flint::ShaderCodeType::SPIR_V);
	shaders[1].LoadFromFile("E:\\Flint\\Assets\\Shaders\\TestShaders\\Default\\shader.frag.spv", Flint::ShaderLocation::FRAGMENT, Flint::ShaderCodeType::SPIR_V);

	return { shaders[0].CreateDigest(), shaders[1].CreateDigest() };
}

void Application::CreateSceneComponent()
{
	// Create the wire frame.
	//mWireFrame = mEngine.CreateNewWireFrame("E:\\Flint\\Assets\\Models\\SkyBox\\SkySphere.obj", Flint::CreateDefaultAttributes());
	//mWireFrame.CreateCache("testCache");
	mWireFrame.LoadFromCache("testCache.wfc", mEngine.GetDevice());

	// Setup the scene component.
	mEngine.SetupSceneComponent(mSceneComponent, mWireFrame, CreateShaderDigests(), GetGraphicsPipelineSpec());
}

void Application::PrepareToRender()
{
	// Setup dynamic states.
	mDynamicState.AddViewPort(Flint::Vector2(1280.0f, 720.0f), Flint::Vector2(0.0f, 1.0f), Flint::Vector2(0.0f, 0.0f));
	mDynamicState.AddScissor(Flint::Vector2(1280.0f, 720.0f), Flint::Vector2(0.0f, 0.0f));

	// Submit the scene component to draw and return the render resource.
	mEngine.SubmitToDrawQueue(mRenderResource, mSceneComponent, mDynamicState);

	// Prepare the render target to render.
	mEngine.PrepareRenderTargetToRender();

	// Get the main MVP uniform buffer.
	mUniformBuffer = mRenderResource.mUniformBuffers["Ubo"];
}

void Application::ProcessInputs()
{
#define CHECK_KEY_INPUT(key)	pInputCenter->key.IsPressed() || pInputCenter->key.IsOnRepeat()

	if (CHECK_KEY_INPUT(KeyW))
		mPosition += Front * MoveFactor;

	if (CHECK_KEY_INPUT(KeyA))
		mPosition -= Left * MoveFactor;

	if (CHECK_KEY_INPUT(KeyS))
		mPosition -= Front * MoveFactor;

	if (CHECK_KEY_INPUT(KeyD))
		mPosition += Left * MoveFactor;
}

void Application::OnUpdate()
{
	// Map the uniform buffer memory to the local address space.
	pDataCopy = static_cast<UniformBufferObject*>(mUniformBuffer.MapMemory(std::numeric_limits<UI32>::max(), 0));

	// Resolve the view matrix.
	Ubo.view = LookAt(mPosition, mPosition + Forward, WorldUp);

	// Copy the data to the uniform buffer.
	*pDataCopy = Ubo;

	// Unmap the mapped buffer.
	mUniformBuffer.FlushMemoryMappings();
	mUniformBuffer.UnmapMemory();
}

void Application::DestroyRenderResources()
{
	// Destroy render resource.
	mSceneComponent.DestroyRenderResources(mRenderResource);

	// Destroy the scene component.
	mEngine.DestroySceneComponent(mSceneComponent);
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
