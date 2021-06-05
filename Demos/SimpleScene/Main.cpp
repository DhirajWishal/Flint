// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Instance.hpp"
#include "Flint/Display.hpp"
#include "Flint/ScreenBoundRenderTarget.hpp"
#include <iostream>

void KeyCallback(Flint::KeyCode key, Flint::EventAction action, Flint::SpecialCharacter character)
{
	if (key == Flint::KeyCode::KEY_A)
		std::cout << std::endl << "Key A is pressed!" << std::endl;
}

int main()
{
	try
	{
		Flint::Instance& mInstance = Flint::CreateInstance(true);
		Flint::Device& mDevice = mInstance.CreateDevice(Flint::DeviceFlags::EXTERNAL | Flint::DeviceFlags::GRAPHICS_COMPATIBLE | Flint::DeviceFlags::COMPUTE_COMPATIBLE);
		Flint::Display& mDisplay = mInstance.CreateDisplay({ 1280, 720 }, "Flint: Sample Scene");

		Flint::ScreenBoundRenderTarget& mRenderTarget = mDevice.CreateScreenBoundRenderTarget(mDisplay, { 1280, 720 }, mDisplay.FindBestBufferCount(mDevice));

		mDisplay.SetKeyCallback(KeyCallback);
		while (mDisplay.IsOpen()) mDisplay.Update();

		mDevice.WaitIdle();
		mDevice.DestroyRenderTarget(mRenderTarget);
		mInstance.DestroyDisplay(mDisplay);
		mInstance.DestroyDevice(mDevice);
		Flint::DestroyInstance(mInstance);
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