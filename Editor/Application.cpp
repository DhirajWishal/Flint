// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.hpp"

#include "GraphicsCore/Instance.hpp"

#include "Engine/AssetLoader.hpp"

Application::Application()
{
	pInstance = Flint::CreateInstance(true);
	pDevice = pInstance->CreateDevice(Flint::DeviceFlags::EXTERNAL | Flint::DeviceFlags::COMPUTE_COMPATIBLE | Flint::DeviceFlags::GRAPHICS_COMPATIBLE);

	// Initialize the main render target.
	mRenderTarget.Initialize(pDevice, pInstance);

	// Test loader.
	{
		auto pShader = pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path("E:\\Flint\\Builds\\Demos\\Binaries\\ShadowMap\\Debug Windows\\Flint\\Shaders\\3D\\shader.vert.spv"));
		auto pGeometryStore = pDevice->CreateGeometryStore(pShader->GetInputAttributes(), sizeof(UI32));

		Flint::VertexDescriptor vDescriptor = {};
		vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::VertexAttributeType::POSITION));
		vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::VertexAttributeType::COLOR_0));
		vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 2, Flint::VertexAttributeType::TEXTURE_COORDINATES_0));

		Flint::AssetLoader loader(pGeometryStore, "E:\\Dynamik\\Game Repository\\assets\\assets\\donut\\Donut.obj", vDescriptor);
		auto wireFrames = loader.GetWireFrames();
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
}
