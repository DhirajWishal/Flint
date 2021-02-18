// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Benchmark/Timer.h"
#include "Flint/Engine.h"
#include "Flint/Components/WireFrame.h"
#include "Flint/Components/ShaderCode.h"

#include <thread>

std::vector<Flint::ShaderCode> GetShaders()
{
	std::vector<Flint::ShaderCode> shaders(2);

	shaders[0].LoadFromFile("E:\\Flint\\Assets\\Shaders\\TestShaders\\Default\\shader.vert", Flint::ShaderLocation::VERTEX, Flint::ShaderCodeType::GLSL);
	shaders[1].LoadFromFile("E:\\Flint\\Assets\\Shaders\\TestShaders\\Default\\shader.frag", Flint::ShaderLocation::FRAGMENT, Flint::ShaderCodeType::GLSL);

	return shaders;
}

int main()
{
	Flint::Benchmark::Timer t;
	Flint::Engine engine = {};
#ifdef FLINT_DEBUG
	engine.Initialize(Flint::BackendAPI::VULKAN, true, 1280, 720, "Flint: Sample Scene");

#else
	engine.Initialize(Flint::BackendAPI::VULKAN, false, 1280, 720, "Flint: Sample Scene");

#endif // FLINT_DEBUG

	engine.CreateRenderTarget(Flint::Vector2(1280, 720));

	auto wireFrame = engine.CreateNewWireFrame("E:\\Flint\\Assets\\Models\\SkyBox\\SkySphere.obj", Flint::CreateDefaultAttributes());

	auto shaders = std::move(GetShaders());
	std::vector<Flint::ShaderDigest> digests;
	for (auto itr = shaders.begin(); itr != shaders.end(); itr++)
		INSERT_INTO_VECTOR(digests, itr->CreateDigest(Flint::ShaderCodeType::SPIR_V));

	engine.CreateSceneComponent(wireFrame, digests, Flint::Backend::GraphicsPipelineSpecification());

	wireFrame.Clear();

	engine.Terminate();
	return 0;
}
