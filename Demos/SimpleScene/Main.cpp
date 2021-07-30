// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Backend/Instance.hpp"
#include "Backend/Display.hpp"
#include "Backend/ScreenBoundRenderTarget.hpp"
#include "Backend/Core/Profiler.hpp"
#include "Backend/Shader.hpp"
#include "Backend/GraphicsPipeline.hpp"
#include "Backend/GeometryStore.hpp"
#include <iostream>

void KeyCallback(Flint::KeyCode key, Flint::EventAction action, Flint::SpecialCharacter character)
{
	if (key == Flint::KeyCode::KEY_A)
		std::cout << std::endl << "Key A is pressed!" << std::endl;
}

std::vector<float> GetVertexes()
{
	std::vector<float> vertexes;
	const float zIndex = 0.5f;

	vertexes.push_back(0.0f);
	vertexes.push_back(0.5f);
	vertexes.push_back(zIndex);
	vertexes.push_back(0.0f);

	vertexes.push_back(0.5f);
	vertexes.push_back(0.0f);
	vertexes.push_back(zIndex);
	vertexes.push_back(0.0f);

	vertexes.push_back(-0.5f);
	vertexes.push_back(-0.5f);
	vertexes.push_back(zIndex);
	vertexes.push_back(0.0f);

	return vertexes;
}

std::vector<UI32> GetIndexes()
{
	std::vector<UI32> indexes;

	indexes.push_back(0);
	indexes.push_back(1);
	indexes.push_back(2);

	//indexes.push_back(2);
	//indexes.push_back(0);

	return indexes;
}

int main()
{
	try
	{
		auto pInstance = Flint::CreateInstance(true);
		auto pDevice = pInstance->CreateDevice(Flint::DeviceFlags::EXTERNAL | Flint::DeviceFlags::GRAPHICS_COMPATIBLE | Flint::DeviceFlags::COMPUTE_COMPATIBLE);
		auto pDisplay = pInstance->CreateDisplay({ 1280, 720 }, "Flint: Sample Scene");

		auto pRenderTarget = pDevice->CreateScreenBoundRenderTarget(pDisplay, { 1280, 720 }, pDisplay->FindBestBufferCount(pDevice));

		auto pBuffer = pDevice->CreateBuffer(Flint::BufferType::STAGGING, 1024);

		auto pVertexShader = pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\DebugGeometry\\shader.vert.spv"), Flint::ShaderCodeType::SPIR_V);
		auto pFragmentShader = pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\DebugGeometry\\shader.frag.spv"), Flint::ShaderCodeType::SPIR_V);
		const auto resourceVS = pVertexShader->GetShaderResources();
		const auto resourceFS = pFragmentShader->GetShaderResources();

		auto pPipeline = pDevice->CreateGraphicsPipeline("TestPipeline", pRenderTarget, pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, Flint::GraphicsPipelineSpecification());

		auto pGeometryStore = pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI32));

		const auto vertexes = GetVertexes();
		const auto indexes = GetIndexes();
		pGeometryStore->AddGeometry(vertexes.size(), vertexes.data(), indexes.size(), indexes.data());
		pPipeline->AddDrawData(pPipeline->CreateResourceMap(), std::make_shared<Flint::DynamicStateContainer>(), 0, vertexes.size(), 0, indexes.size());
		pRenderTarget->SubmitPipeline(pGeometryStore, pPipeline);

		pDisplay->SetKeyCallback(KeyCallback);
		pRenderTarget->PrepareStaticResources();

		auto resourceMap = pPipeline->GetDrawData();

		while (pDisplay->IsOpen())
		{
			FLINT_SETUP_PROFILER();
			pDisplay->Update();

			pRenderTarget->BeginFrame();
			pRenderTarget->Update();
			pRenderTarget->SubmitFrame();
		}

		pDevice->DestroyGeometryStore(pGeometryStore);
		pDevice->DestroyShader(pVertexShader);
		pDevice->DestroyShader(pFragmentShader);

		pDevice->DestroyPipeline(pPipeline);

		pDevice->DestroyBuffer(pBuffer);

		pDevice->DestroyRenderTarget(pRenderTarget);

		pInstance->DestroyDisplay(pDisplay);
		pInstance->DestroyDevice(pDevice);
		Flint::DestroyInstance(pInstance);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

/**
 * Flint::Instance& mInstance = Flint::CreateInstance(Flint::BackendAPI::VULKAN, true);
 * Flint::Device& mDevice = mInstance.CreateDevice();
 * Flint::Display& mDisplay = mInstance.CreateDisplay(1280, 720, "Example");
 *
 * Flint::ScreenBoundRenderTarget& mRenderTarget = mDevice.CreateScreenBoundRenderTarget(mDevice, {1280, 720}, FLINT_COLOR_BUFFER | FLINT_DEPTH_BUFFER, 0);
 *
 * Flint::VertexShader& mVertexShader = mDevice.CreateVertexShader(GetVertexShaderPath());
 * Flint::FragmentShader& mFragmentShader = mDevice.CreateFragmentShader(GetFragmentShaderPath());
 *
 * Flint::GraphicsPipeline& mPipeline = mDevice.CreateGraphicsPipeline(mVertexShader, mFragmentShader);
 *
 * Flint::VertexBuffer& mVertexBuffer = mDevice.CreateVertexBuffer(GetVertexBufferSize());
 * mVertexBuffer.SubmitData(GetVertexPointer(), GetVertexBufferSize(), 0);
 *
 * Flint::IndexBuffer& mIndexBuffer = mDevice.CreateIndexBuffer(GetIndexBufferSize());
 * mIndexBuffer.SubmitData(GetIndexPointer(), GetIndexBufferSize(), 0);
 *
 * mPipeline.AddDrawInstance(mVertexBuffer, mIndexBuffer, GetDrawData());
 *
 * mDevice.DestroyBuffer(mIndexBuffer);
 * mDevice.DestroyBuffer(mVertexBuffer);
 *
 * mDevice.DestroyPipeline(mPipeline);
 *
 * mDevice.DestroyShader(mFragmentShader);
 * mDevice.DestroyShader(mVertexShader);
 *
 * mDevice.DestroyRenderTarget(mRenderTarget);
 *
 * mInstance.DestroyDevice(mDevice);
 * mInstance.DestroyDisplay(mDisplay);
 *
 * Flint::DestroyInstance(mInstance);
 */