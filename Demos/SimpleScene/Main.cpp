// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0


#include "Core/Benchmark/Timer.h"

#include <thread>

int main()
{
//	Flint::Benchmark::Timer t;
//	Flint::LowLevel::SetupBackend(Flint::LowLevel::BackendAPI::VULKAN);
//
//	Flint::LowLevel::Instance instance = {};
//
//#ifdef FLINT_DEBUG
//	instance.Initialize(true);
//
//#else
//	instance.Initialize(false);
//
//#endif // FLINT_DEBUG
//
//	Flint::LowLevel::Display display = {};
//	display.Initialize(instance, 1280, 720, "Flint Demo: Simple Scene");
//
//	Flint::LowLevel::Device device = {};
//	device.Initialize(display);
//
//	Flint::LowLevel::RenderTargetSB3D renderTarget = {};
//	renderTarget.Initialize(device, Flint::Vector2(1280, 720));
//
//	Flint::ShaderCode vShaderCode;
//	vShaderCode.Initialize("E:\\Dynamik\\Version\\Dynamik\\Assets\\Shaders\\3D\\vert.spv", Flint::ShaderLocation::VERTEX, Flint::ShaderCodeType::SPIR_V);
//
//	Flint::ShaderCode fShaderCode;
//	fShaderCode.Initialize("E:\\Dynamik\\Version\\Dynamik\\Assets\\Shaders\\3D\\frag.spv", Flint::ShaderLocation::FRAGMENT, Flint::ShaderCodeType::SPIR_V);
//
//	Flint::LowLevel::GraphicsPipeline gPipeline = {};
//	gPipeline.Initialize(renderTarget, { vShaderCode, fShaderCode });
//
//	//std::this_thread::sleep_for(std::chrono::microseconds(5000000));
//	gPipeline.Terminate();
//
//	renderTarget.Terminate();
//	device.Terminate();
//
//	display.Terminate();
//	instance.Terminate();

	return 0;
}

/**
 * auto pipeline  = renderTarget.CreateGraphicsPipeline({ vShaderD, fShaderD });
 *
 * auto uniformMVP = CreateUniformMVP("uniformMVP", GetDefaultBinding());
 * auto image = CreateImage("sampledImage", GetImageBinding());
 *
 * pRenderTarget->AddToRenderQueue(pipeline, { uniformMVP }, { image }, vBuffer, iBuffer);
 */