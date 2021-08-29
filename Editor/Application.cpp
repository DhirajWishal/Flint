// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.hpp"

#include "GraphicsCore/Instance.hpp"

#include "Engine/Asset.hpp"
#include "Engine/ShaderCompiler.hpp"

Application::Application()
{
	pInstance = Flint::CreateInstance(true);
	pDevice = pInstance->CreateDevice(Flint::DeviceFlags::EXTERNAL | Flint::DeviceFlags::COMPUTE_COMPATIBLE | Flint::DeviceFlags::GRAPHICS_COMPATIBLE);

	// Initialize the main render target.
	mRenderTarget.Initialize(pDevice, pInstance);

	// Test loader.
	{
		Flint::ShaderCompiler compiler(std::filesystem::path("E:\\Flint\\Editor\\Shaders\\3D\\shader.vert"), Flint::ShaderCodeType::GLSL, Flint::ShaderType::VERTEX);
		auto pShader = pDevice->CreateShader(Flint::ShaderType::VERTEX, compiler.GetShaderCode());
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
	mRenderTarget.DrawFrame();
}
