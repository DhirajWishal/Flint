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
const float rotationBias = 0.0005f;

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
			mCamera.Rotate(rotationBias, 0);
		else
			mCamera.Rotate(-rotationBias, 0);

	}

	// Rotate y
	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).IsOnRepeat())
	{
		if ((pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Y).GetSpecialCharacter() & Flint::SpecialCharacter::SHIFT) == Flint::SpecialCharacter::SHIFT)
			mCamera.Rotate(rotationBias, 1);
		else
			mCamera.Rotate(-rotationBias, 1);
	}

	// Rotate z
	if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).IsPressed() || pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).IsOnRepeat())
	{
		if ((pDisplay->GetKeyEvent(Flint::KeyCode::KEY_Z).GetSpecialCharacter() & Flint::SpecialCharacter::SHIFT) == Flint::SpecialCharacter::SHIFT)
			mCamera.Rotate(rotationBias, 2);
		else
			mCamera.Rotate(-rotationBias, 2);
	}
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

		const auto [vertexes, indexes] = LoadAsset("E:\\Dynamik\\Game Repository\\assets\\assets\\VikingRoom\\untitled.obj");
		auto image = LoadImage("E:\\Dynamik\\Game Repository\\assets\\assets\\VikingRoom\\texture.png");
		UI32 mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(image.mExtent.mWidth, image.mExtent.mHeight)))) + 1;
		auto pImage = pDevice->CreateImage(Flint::ImageType::DIMENSIONS_2, Flint::ImageUsage::GRAPHICS, image.mExtent, Flint::PixelFormat::R8G8B8A8_SRGB, 1, mipLevels, image.pImageData);
		auto pSampler = pDevice->CreateImageSampler(Flint::ImageSamplerSpecification());
		DestroyImage(image);

		auto pVertexShader = pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\3D\\shader.vert.spv"), Flint::ShaderCodeType::SPIR_V);
		auto pFragmentShader = pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path("E:\\Flint\\Assets\\Shaders\\3D\\shader.frag.spv"), Flint::ShaderCodeType::SPIR_V);
		const auto resourceVS = pVertexShader->GetShaderResources();
		const auto resourceFS = pFragmentShader->GetShaderResources();

		auto pPipeline = pDevice->CreateGraphicsPipeline("TestPipeline", pRenderTarget, pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, Flint::GraphicsPipelineSpecification());

		auto pGeometryStore = pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI32));
		auto pResourceMap = pPipeline->CreateResourceMap();
		pResourceMap->SetResource("Ubo", pBuffer);
		pResourceMap->SetResource("texSampler", pSampler, pImage);

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
			{
				auto extent = pDisplay->GetExtent();
				if (extent.mWidth > 0 && extent.mHeight > 0)
					mCamera.SetAspectRatio(extent);
			}

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

		pDevice->DestroyImage(pImage);
		pDevice->DestroyImageSampler(pSampler);
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