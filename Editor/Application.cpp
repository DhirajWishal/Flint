// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.hpp"

#include "GraphicsCore/Instance.hpp"

#include "Engine/Asset.hpp"
#include "Engine/ShaderCompiler.hpp"

Application::Application()
{
	pInstance = Flint::CreateInstance(true);
	pDevice = pInstance->CreateDevice(Flint::DeviceFlags::External | Flint::DeviceFlags::ComputeCompatible | Flint::DeviceFlags::GraphicsCompatible);

	// Initialize the main render target.
	mRenderTarget.Initialize(pDevice, pInstance);

	// Test loader.
	{
		Flint::ShaderCompiler compiler(std::filesystem::path("E:\\Flint\\Editor\\Shaders\\3D\\shader.vert"), Flint::ShaderCodeType::GLSL, Flint::ShaderType::Vertex);
		auto pShader = pDevice->CreateShader(Flint::ShaderType::Vertex, compiler.GetShaderCode());
		auto pGeometryStore = pDevice->CreateGeometryStore(pShader->GetInputAttributes(), sizeof(UI32));
	
		Flint::Asset asset("E:\\Demo\\Vulkan\\data\\models\\voyager.gltf", pGeometryStore, Flint::Defaults::CreateDefaultVertexDescriptor());
	}

	// Main execution loop.
	while (mRenderTarget.IsDisplayOpen() && bShouldRun)
		Execute();
}

Application::~Application()
{
	mRenderTarget.Terminate();

	pDevice->Terminate();
	pInstance->Terminate();
}

void Application::Execute()
{
	mRenderTarget.PollEvents();

	mNewTimePoint = std::chrono::steady_clock::now();
	UI64 delta = now.time_since_epoch().count() - mOldTimePoint.time_since_epoch().count();

	mState.PrepareNewFrame();

	ImGui::NewFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();

	float frameTime = delta / (1000.0f * 1000.0f);
	average += frameTime;
	counter++;

	ImGui::Text("Frame Time: %f ms", frameTime);
	ImGui::Text("Average Frame Time: %f ms", average / counter);

	mRenderTarget.DrawFrame();

	mOldTimePoint = mNewTimePoint;
}
