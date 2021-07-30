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

#include "Components/Camera.hpp"
#include "Components/AssetLoader.hpp"

glm::vec3 rotations = glm::vec3(0.0f);
const float rotationBias = 0.0000005f;

Camera mCamera = {};
void KeyCallback(std::shared_ptr<Flint::Display> pDisplay)
{
	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_W).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_W).IsOnRepeat())
		mCamera.WalkUp();

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_A).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_A).IsOnRepeat())
		mCamera.WalkLeft();

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_S).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_S).IsOnRepeat())
		mCamera.WalkDown();

	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_D).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_D).IsOnRepeat())
		mCamera.WalkRight();

	if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsPressed())
		mCamera.MousePosition(pDisplay->GetMousePosition());

	if (pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsReleased())
		mCamera.ResetFirstMouse();

	// Rotate x
	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_X).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_X).IsOnRepeat())
	{
		if ((pDisplay->GetKeyEvent(Flint::KeyCode::KEY_X).GetSpecialCharacter() & Flint::SpecialCharacter::SHIFT) == Flint::SpecialCharacter::SHIFT)
			rotations += glm::vec3(1.0f, 0.0f, 0.0f) * rotationBias;
		else
			rotations -= glm::vec3(1.0f, 0.0f, 0.0f) * rotationBias;

		mCamera.Rotate(rotations[0], 0);
	}

	// Rotate y
	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).IsOnRepeat())
	{
		if ((pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).GetSpecialCharacter() & Flint::SpecialCharacter::SHIFT) == Flint::SpecialCharacter::SHIFT)
			rotations += glm::vec3(0.0f, 1.0f, 0.0f) * rotationBias;
		else
			rotations -= glm::vec3(0.0f, 1.0f, 0.0f) * rotationBias;

		mCamera.Rotate(rotations[1], 1);
	}

	// Rotate z
	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).IsOnRepeat())
	{
		if ((pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).GetSpecialCharacter() & Flint::SpecialCharacter::SHIFT) == Flint::SpecialCharacter::SHIFT)
			rotations += glm::vec3(0.0f, 0.0f, 1.0f) * rotationBias;
		else
			rotations -= glm::vec3(0.0f, 0.0f, 1.0f) * rotationBias;

		mCamera.Rotate(rotations[2], 2);
	}
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

		auto pBuffer = pDevice->CreateBuffer(Flint::BufferType::UNIFORM, sizeof(ModelViewProjection));

		auto pVertexShader = pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\DebugGeometry\\shader.vert.spv"), Flint::ShaderCodeType::SPIR_V);
		auto pFragmentShader = pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\DebugGeometry\\shader.frag.spv"), Flint::ShaderCodeType::SPIR_V);
		const auto resourceVS = pVertexShader->GetShaderResources();
		const auto resourceFS = pFragmentShader->GetShaderResources();

		auto pPipeline = pDevice->CreateGraphicsPipeline("TestPipeline", pRenderTarget, pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, Flint::GraphicsPipelineSpecification());

		auto pGeometryStore = pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI32));
		auto pResourceMap = pPipeline->CreateResourceMap();
		pResourceMap->SetResource("Ubo", pBuffer);

		const auto [vertexes, indexes] = LoadAsset("E:\\Dynamik\\Game Repository\\assets\\assets\\chalet\\chalet.obj");
		pGeometryStore->AddGeometry(vertexes.size(), vertexes.data(), indexes.size(), indexes.data());
		pPipeline->AddDrawData(pResourceMap, std::make_shared<Flint::DynamicStateContainer>(), 0, vertexes.size(), 0, indexes.size());
		pRenderTarget->SubmitPipeline(pGeometryStore, pPipeline);

		pRenderTarget->PrepareStaticResources();

		auto resourceMap = pPipeline->GetDrawData();

		mCamera.SetAspectRatio(pDisplay->GetExtent());
		while (pDisplay->IsOpen())
		{
			FLINT_SETUP_PROFILER();
			if (pDisplay->IsDisplayResized())
				mCamera.SetAspectRatio(pDisplay->GetExtent());

			KeyCallback(pDisplay);

			mCamera.Update();

			ModelViewProjection matrix = mCamera.GetModelViewProjection();
			BYTE* pData = static_cast<BYTE*>(pBuffer->MapMemory(sizeof(ModelViewProjection)));
			std::copy(reinterpret_cast<BYTE*>(&matrix), reinterpret_cast<BYTE*>(&matrix) + sizeof(ModelViewProjection), pData);
			pBuffer->UnmapMemory();

			pRenderTarget->BeginFrame();
			pRenderTarget->Update();
			pRenderTarget->SubmitFrame();

			pDisplay->Update();
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