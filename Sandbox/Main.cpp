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

#include "Firefly/TerrainBuilder.hpp"

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
[[nodiscard]] Flint::Backend::RasterizingPipelineSpecification GetDefaultSpecification()
{
	Flint::Backend::RasterizingPipelineSpecification specification;
	return specification;
}

int main()
{
	auto builder = TerrainBuilder(rand());
	auto block = builder.update(0, 0);

	auto instance = Flint::CreateInstance("Sandbox", 1, Validation);
	auto device = instance->createDevice();

	auto window = device->createWindow("Sandbox", 1280, 720);
	auto camera = Flint::MonoCamera(glm::vec3(0.0f), window->getWidth(), window->getHeight());
	camera.m_MovementBias = 10;
	//camera.m_RotationBias = 50;

	auto rasterizer = device->createRasterizer(camera, window->getFrameCount(), { Flint::Backend::Defaults::ColorAttachmentDescription, Flint::Backend::Defaults::DepthAttachmentDescription });
	auto rayTracer = device->createRayTracer(camera, window->getFrameCount());
	auto model = device->createStaticModel(std::filesystem::path(FLINT_GLTF_ASSET_PATH) / "Sponza" / "glTF" / "Sponza.gltf");
	//auto model = device->createStaticModel("E:\\Assets\\Sponza\\Main\\Main\\NewSponza_Main_FBX_ZUp.fbx");

	// Set the resize callback.
	window->setResizeCallback([&camera, rasterizer, rayTracer](uint32_t width, uint32_t height)
		{
			camera.m_FrameWidth = width;
			camera.m_FrameHeight = height;

			rasterizer->updateExtent();
			rayTracer->updateExtent();
		}
	);

	// The default texture to make sure that we have a default one to fall back to.
	struct { uint8_t r = 255, g = 255, b = 255, a = 255; } defaultImage;
	auto defaultTexture = device->createTexture2D(1, 1, Flint::ImageUsage::Graphics, Flint::PixelFormat::R8G8B8A8_SRGB, 1, Flint::Multisample::One, reinterpret_cast<const std::byte*>(&defaultImage));
	Flint::StaticStorage<std::shared_ptr<Flint::Backend::Texture2D>>::Set("Default", defaultTexture);
	Flint::StaticStorage<std::shared_ptr<Flint::Backend::TextureView>>::Set("Default", defaultTexture->createView());

	auto cameraBuffer = camera.createBuffer(device);
	auto program = device->createRasterizingProgram(Flint::Backend::ShaderCode("Shaders/Debugging/Shader.vert.spv"), Flint::Backend::ShaderCode("Shaders/Debugging/Shader.frag.spv"));
	auto defaultPipeline = rasterizer->createPipeline(program, GetDefaultSpecification(), std::make_unique<Flint::Backend::Defaults::FilePipelineCacheHandler>("PipelineCache/"));
	auto drawEntry = defaultPipeline->attach(model, [cameraBuffer, device](auto& model, const Flint::Backend::StaticMesh& mesh, const Flint::Backend::BindingMap& binder)
		{
			Flint::Backend::MeshBindingTable table;

			for (const auto& binding : binder.getBindings())
			{
				if (binding.m_Name == "camera")
				{
					table.bind(binding.m_BindingIndex, cameraBuffer);
				}
				else if (binding.m_Name == "baseColorTexture")
				{
					constexpr auto textureIndex = Flint::EnumToInt(Flint::Backend::TextureType::BaseColor);
					const auto& texturePath = mesh.m_TexturePaths[textureIndex];
					const auto texturePathString = texturePath.string();

					// Load the texture file if we haven't already.
					if (!Flint::StaticStorage<std::shared_ptr<Flint::Backend::Texture2D>>::Contains(texturePathString) && texturePath.has_filename())
					{
						auto pTexture = Flint::Backend::Texture2D::LoadFromFile(device, texturePath, Flint::ImageUsage::Graphics);
						Flint::StaticStorage<std::shared_ptr<Flint::Backend::Texture2D>>::Set(texturePathString, pTexture);
						Flint::StaticStorage<std::shared_ptr<Flint::Backend::TextureView>>::Set(texturePathString, pTexture->createView());
					}

					// Now we can bind if possible.
					if (Flint::StaticStorage<std::shared_ptr<Flint::Backend::Texture2D>>::Contains(texturePathString))
					{
						auto pTexture = Flint::StaticStorage<std::shared_ptr<Flint::Backend::Texture2D>>::Get(texturePathString);

						// Create the view if not available.
						if (!Flint::StaticStorage<std::shared_ptr<Flint::Backend::TextureView>>::Contains(texturePathString))
							Flint::StaticStorage<std::shared_ptr<Flint::Backend::TextureView>>::Set(texturePathString, pTexture->createView());

						Flint::TextureSamplerSpecification samplerSpecification;
						samplerSpecification.m_MaxLevelOfDetail = pTexture->getMipLevels();

						// Internally it caches so we don't need to store things anywhere.
						table.bind(binding.m_BindingIndex, Flint::StaticStorage<std::shared_ptr<Flint::Backend::TextureView>>::Get(texturePathString), device->createTextureSampler(std::move(samplerSpecification)), Flint::ImageUsage::Graphics);
					}
					else
					{
						table.bind(binding.m_BindingIndex, Flint::StaticStorage<std::shared_ptr<Flint::Backend::TextureView>>::Get("Default"), device->createTextureSampler(Flint::TextureSamplerSpecification()), Flint::ImageUsage::Graphics);
					}
				}
			}

			return table;
		}
	);

	const auto firstInstance = drawEntry->instance();	// First instance.

	window->attach(rasterizer);
	//window->attach(rayTracer);

	bool firstMouse = true;
	float lastX = 0.0f, lastY = 0.0f;

	Flint::FrameTimer timer;
	while (!g_EventSystem.shouldClose())
	{
		const auto events = g_EventSystem.poll();
		const auto duration = timer.tick();

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
		else if (events == Flint::EventType::Mouse)
		{
			if (g_EventSystem.getMouse().m_Left)
			{
				const auto positionX = g_EventSystem.getMouse().m_PositionX * -1.0f;
				const auto positionY = g_EventSystem.getMouse().m_PositionY * -1.0f;

				if (firstMouse)
				{
					lastX = positionX;
					lastY = positionY;
					firstMouse = false;
				}

				constexpr float sensitivity = 0.05f;
				const float xoffset = (positionX - lastX) * sensitivity * 0.75f;
				const float yoffset = (lastY - positionY) * sensitivity; // Reversed since y-coordinates go from bottom to top

				lastX = positionX;
				lastY = positionY;

				camera.m_Yaw += xoffset;
				camera.m_Pitch += yoffset;

				if (camera.m_Pitch > 89.0f) camera.m_Pitch = 89.0f;
				if (camera.m_Pitch < -89.0f) camera.m_Pitch = -89.0f;
			}
			else
				firstMouse = true;
		}

		//spdlog::info("Frame rate: {}", Flint::FrameTimer::FramesPerSecond(duration), " ns");
		camera.update();
		camera.copyToBuffer(cameraBuffer);

		rasterizer->update();	// Even though the rasterizer is attached as a dependency, we still need to manually update it.
		rayTracer->update();
		window->update();
	}

	device->waitIdle();	// Wait till we finish prior things before we proceed.

	Flint::StaticStorage<std::shared_ptr<Flint::Backend::Texture2D>>::Clear();
	Flint::StaticStorage<std::shared_ptr<Flint::Backend::TextureView>>::Clear();

	defaultTexture->terminate();
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