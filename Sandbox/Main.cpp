// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/EventSystem/EventSystem.hpp"
#include "Core/Camera/MonoCamera.hpp"

#include "Core/Window.hpp"
#include "Core/Rasterizer.hpp"
#include "Core/RayTracer.hpp"
#include "Core/RasterizingProgram.hpp"
#include "Core/StaticModel.hpp"

#include "Engine/Utility/FrameTimer.hpp"
#include "Engine/Flint.hpp"
#include "Engine/StaticStorage.hpp"

#ifdef FLINT_DEBUG
constexpr auto Validation = true;

#else
constexpr auto Validation = false;

#endif

// We need to do this because of SDL.
#ifdef main
#	undef main

#endif

// Globals.
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

	auto cameraBuffer = camera.createBuffer(device);
	auto defaultPipeline = rasterizer->createPipeline(program, GetDefaultSpecification(), std::make_unique<Flint::Defaults::FilePipelineCacheHandler>("PipelineCache/"));
	auto drawEntry = defaultPipeline->attach(model, [cameraBuffer, device](auto& model, const Flint::StaticMesh& mesh, auto& binder)
		{
			Flint::MeshBindingTable table;
			table.bind(0, cameraBuffer);

			constexpr auto textureIndex = Flint::EnumToInt(Flint::TextureType::BaseColor);
			const auto& texturePath = mesh.m_TexturePaths[textureIndex];
			const auto texturePathString = texturePath.string();

			// Load the texture file if we haven't already.
			if (!Flint::StaticStorage<std::shared_ptr<Flint::Texture2D>>::Contains(texturePathString) && texturePath.has_filename())
			{
				auto pTexture = Flint::Texture2D::LoadFromFile(device, texturePath, Flint::ImageUsage::Graphics);
				Flint::StaticStorage<std::shared_ptr<Flint::Texture2D>>::Set(texturePathString, pTexture);
				Flint::StaticStorage<std::shared_ptr<Flint::TextureView>>::Set(texturePathString, pTexture->createView());
			}

			// Now we can bind if possible.
			if (Flint::StaticStorage<std::shared_ptr<Flint::Texture2D>>::Contains(texturePathString))
			{
				auto pTexture = Flint::StaticStorage<std::shared_ptr<Flint::Texture2D>>::Get(texturePathString);

				// Create the view if not available.
				if (!Flint::StaticStorage<std::shared_ptr<Flint::TextureView>>::Contains(texturePathString))
					Flint::StaticStorage<std::shared_ptr<Flint::TextureView>>::Set(texturePathString, pTexture->createView());

				// Internally it caches so we don't need to store things anywhere.
				table.bind(1, Flint::StaticStorage<std::shared_ptr<Flint::TextureView>>::Get(texturePathString), device->createTextureSampler(Flint::TextureSamplerSpecification()), Flint::ImageUsage::Graphics);
			}

			return table;
		}
	);
	auto firstInstance = drawEntry->instance();	// First instance.

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
		camera.copyToBuffer(cameraBuffer);

		rasterizer->update();	// Even though the rasterizer is attached as a dependency, we still need to manually update it.
		rayTracer->update();
		window->update();
	}

	const auto ss = timer.tick();

	device->waitIdle();	// Wait till we finish prior things before we proceed.

	Flint::StaticStorage<std::shared_ptr<Flint::Texture2D>>::Clear();
	Flint::StaticStorage<std::shared_ptr<Flint::TextureView>>::Clear();

	cameraBuffer->terminate();
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