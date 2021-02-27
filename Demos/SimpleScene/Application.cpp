// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.h"

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
	mEngine.CreateRenderTarget(Flint::Vector2(static_cast<float>(mWidth), static_cast<float>(mHeight)));

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
	mWireFrame = mEngine.CreateNewWireFrame("E:\\Flint\\Assets\\Models\\SkyBox\\SkySphere.obj", Flint::CreateDefaultAttributes());

	// Create the scene component.
	mSceneComponent = mEngine.CreateSceneComponent(mWireFrame, CreateShaderDigests(), GetGraphicsPipelineSpec());
}

void Application::PrepareToRender()
{
	// Setup dynamic states.
	mDynamicState.AddViewPort({ 1280.0f, 720.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f });
	mDynamicState.AddScissor({ 1280.0f, 720.0f }, { 0.0f, 0.0f });

	// Submit the scene component to draw and return the render resource.
	mRenderResource = mEngine.SubmitToDrawQueue(mSceneComponent, mDynamicState);

	// Prepare the render target to render.
	mEngine.PrepareRenderTargetToRender();

	// Get the main MVP uniform buffer.
	pUniformBuffer = mRenderResource.mUniformBuffers["Ubo"];
}

void Application::ProcessInputs()
{
#define CHECK_KEY_INPUT(key)	pInputCenter->key.IsPressed() || pInputCenter->key.IsOnRepeat()

	if (CHECK_KEY_INPUT(KeyW))
		mPosition += Flint::Vector3(1.0f, 0.0f, 0.0f) * 0.001f;

	if (CHECK_KEY_INPUT(KeyS))
		mPosition -= Flint::Vector3(1.0f, 0.0f, 0.0f) * 0.001f;

	if (CHECK_KEY_INPUT(KeyA))
		mPosition += Flint::Vector3(0.0f, 0.0f, 1.0f) * 0.001f;

	if (CHECK_KEY_INPUT(KeyD))
		mPosition -= Flint::Vector3(0.0f, 0.0f, 1.0f) * 0.001f;
}

void Application::OnUpdate()
{
	// Map the uniform buffer memory to the local address space.
	pDataCopy = static_cast<UniformBufferObject*>(pUniformBuffer->MapMemory(std::numeric_limits<UI32>::max(), 0));

	// Resolve the view matrix.
	Ubo.view = LookAt(mPosition, mPosition + Flint::Vector3(1.0f, 0.0f, 0.0f), Flint::Vector3(0.0f, 1.0f, 0.0f));

	// Copy the data to the uniform buffer.
	*pDataCopy = Ubo;

	// Unmap the mapped buffer.
	pUniformBuffer->FlushMemoryMappings();
	pUniformBuffer->UnmapMemory();
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

	return spec;
}
