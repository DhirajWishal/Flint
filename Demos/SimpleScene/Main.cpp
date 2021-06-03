// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Instance.hpp"
#include <iostream>

int main()
{
	try
	{
		Flint::Instance& mInstance = Flint::CreateInstance(true);
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