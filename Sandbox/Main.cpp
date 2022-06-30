// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/EventSystem/EventSystem.hpp"
#include "Core/Camera/MonoCamera.hpp"
#include "Engine/Utility/FrameTimer.hpp"
#include "Engine/Flint.hpp"

#include "Core/Window.hpp"
#include "Core/Rasterizer.hpp"
#include "Core/RayTracer.hpp"
#include "Core/RasterizingProgram.hpp"
#include "Core/StaticModel.hpp"

#ifdef FLINT_DEBUG
constexpr auto Validation = true;

#else
constexpr auto Validation = false;

#endif

// We need to do this because of SDL.
#ifdef main
#	undef main

#endif

// Global event system instance.
static Flint::EventSystem g_EventSystem;

/**
 * Get the default pipeline specification.
 *
 * @return The specification.
 */
[[nodiscard]] Flint::RasterizingPipelineSpecification GetDefaultSpecification()
{
	Flint::RasterizingPipelineSpecification specification;
	return specification;
}

int main()
{
	auto instance = Flint::CreateInstance("Sandbox", 1, Validation);
	auto device = instance->createDevice();

	auto camera = Flint::MonoCamera(glm::vec3(0.0f), 1280, 720);
	camera.m_MovementBias = 50;

	auto program = device->createRasterizingProgram(Flint::ShaderCode("Shaders/Debugging/vert.spv"), Flint::ShaderCode("Shaders/Debugging/frag.spv"));
	auto window = device->createWindow("Sandbox");
	auto rasterizer = device->createRasterizer(camera, window->getFrameCount(), { Flint::Defaults::ColorAttachmentDescription, Flint::Defaults::DepthAttachmentDescription });
	auto rayTracer = device->createRayTracer(camera, window->getFrameCount());
	auto model = device->createStaticModel(std::filesystem::path(FLINT_GLTF_ASSET_PATH) / "Sponza" / "glTF" / "Sponza.gltf");

	auto defaultPipeline = rasterizer->createPipeline(program, GetDefaultSpecification(), std::make_unique<Flint::Defaults::FilePipelineCacheHandler>("Cache/default.fpc"));

	// auto occlusionPipeline = rasterizer->createPipeline(occlusionProgram, getOcclusionSpecification());
	// auto drawEntry = occlusionPipeline->register(model);
	// drawEntry->instance(position(), rotation(), scale());

	window->attach(rasterizer);
	//window->attach(rayTracer);

	Flint::FrameTimer timer;
	while (!g_EventSystem.shouldClose())
	{
		const auto duration = timer.tick();
		const auto events = g_EventSystem.poll();

		if (events == Flint::EventType::Keyboard)
		{
			if (g_EventSystem.getKeyboard().m_KeyW)
				camera.moveForward(duration.count());

			if (g_EventSystem.getKeyboard().m_KeyS)
				camera.moveBackward(duration.count());

			if (g_EventSystem.getKeyboard().m_KeyA)
				camera.moveLeft(duration.count());

			if (g_EventSystem.getKeyboard().m_KeyD)
				camera.moveRight(duration.count());
		}

		//spdlog::info("Frame rate: {}", Flint::FrameTimer::FramesPerSecond(duration), " ns");
		camera.update();
		rasterizer->update();	// Even though the rasterizer is attached as a dependency, we still need to manually update it.
		rayTracer->update();
		window->update();
	}

	const auto ss = timer.tick();

	defaultPipeline->terminate();
	model->terminate();
	rayTracer->terminate();
	rasterizer->terminate();
	program->terminate();

	window->terminate();

	device->terminate();
	instance->terminate();

	return 0;
}