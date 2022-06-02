// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanWindow.hpp"
#include "VulkanBackend/VulkanRasterizer.hpp"
#include "Core/EventSystem/EventSystem.hpp"
#include "Engine/Camera/MonoCamera.hpp"
#include "Engine/Utility/FrameTimer.hpp"

#include "Engine/GraphicsEngine.hpp"

#ifdef FLINT_DEBUG
constexpr auto Validation = true;

#else
constexpr auto Validation = false;

#endif

// We need to do this because of SDL.
#ifdef main
#	undef main

#endif

[[nodiscard]] Flint::RasterizingPipelineSpecification GetSpecification()
{
	Flint::RasterizingPipelineSpecification specification;
	specification.m_CacheFile = "Debugging.bin";
	specification.m_VertexShader = Flint::VertexShader("Shaders/Debugging/vert.spv");
	specification.m_FragmentShader = Flint::FragmentShader("Shaders/Debugging/frag.spv");

	specification.m_VertexShader.addVertexInput(0, Flint::VertexAttribute::Position, Flint::DataType::Vec3_32);	// layout (location = 0) in vec3 inPosition;
	specification.m_VertexShader.addVertexInput(1, Flint::VertexAttribute::Normal, Flint::DataType::Vec3_32);	// layout (location = 1) in vec3 inNormal;
	specification.m_VertexShader.addVertexInput(2, Flint::VertexAttribute::Texture0, Flint::DataType::Vec2_32);	// layout (location = 2) in vec2 inTextureCoordinates;

	return specification;
}

int main()
{
	auto engine = Flint::GraphicsEngine("Sandbox", 1, Validation, Flint::BackendAPI::Vulkan);
	auto device = engine.createDevice();
	engine.destroyDevice(device.get());

	auto eventSystem = Flint::EventSystem();

	{
		auto camera = Flint::MonoCamera(glm::vec3(0.0f), static_cast<float>(1280) / 720);
		camera.m_MovementBias = 50;

		Flint::FrameTimer timer;
		while (!eventSystem.shouldClose())
		{

			const auto duration = timer.tick();
			const auto events = eventSystem.poll();

			if (events == Flint::EventType::Keyboard)
			{
				if (eventSystem.keyboard().m_KeyW)
					camera.moveForward(duration.count());

				if (eventSystem.keyboard().m_KeyS)
					camera.moveBackward(duration.count());

				if (eventSystem.keyboard().m_KeyA)
					camera.moveLeft(duration.count());

				if (eventSystem.keyboard().m_KeyD)
					camera.moveRight(duration.count());
			}

			camera.update();
		}

		const auto ss = timer.tick();
	}

	return 0;
}