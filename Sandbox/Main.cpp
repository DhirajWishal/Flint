// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/EventSystem/EventSystem.hpp"
#include "Core/Camera/MonoCamera.hpp"
#include "Engine/Utility/FrameTimer.hpp"

#include "VulkanBackend/Exporter.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/chrono.h>

#ifdef FLINT_DEBUG
constexpr auto Validation = true;

#else
constexpr auto Validation = false;

#endif

// We need to do this because of SDL.
#ifdef main
#	undef main

#endif

static Flint::EventSystem g_EventSystem;

int main()
{
	auto instance = Flint::Backend::Instance("Sandbox", 1, Validation);

	{
		auto device = Flint::Backend::Device(instance);

		{
			auto window = Flint::Backend::Window(device, "Sandbox");
			auto camera = Flint::MonoCamera(glm::vec3(0.0f), window.getWidth(), window.getHeight());
			camera.m_MovementBias = 50;

			Flint::FrameTimer timer;

			auto program = Flint::Backend::RasterizingProgram(device, "Shaders/Debugging/vert.spv", "Shaders/Debugging/frag.spv");
			auto rasterizer = Flint::Backend::Rasterizer(device, camera, window.getFrameCount(), { Flint::Core::Defaults::ColorAttachmentDescription, Flint::Core::Defaults::DepthAttachmentDescription });
			window.attach(&rasterizer, 0);

			while (!g_EventSystem.shouldClose())
			{
				const auto duration = timer.tick();
				const auto events = g_EventSystem.poll();

				if (events == Flint::EventType::Keyboard)
				{
					if (g_EventSystem.keyboard().m_KeyW)
						camera.moveForward(duration.count());

					if (g_EventSystem.keyboard().m_KeyS)
						camera.moveBackward(duration.count());

					if (g_EventSystem.keyboard().m_KeyA)
						camera.moveLeft(duration.count());

					if (g_EventSystem.keyboard().m_KeyD)
						camera.moveRight(duration.count());
				}

				//spdlog::info("Frame rate: {}", Flint::FrameTimer::FramesPerSecond(duration), " ns");
				camera.update();
				rasterizer.update();
				window.update();
			}

			const auto ss = timer.tick();
		}
	}

	return 0;
}