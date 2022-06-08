// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/EventSystem/EventSystem.hpp"
#include "Engine/Camera/MonoCamera.hpp"
#include "Engine/Utility/FrameTimer.hpp"

#include "Engine/GraphicsEngine.hpp"

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
	auto engine = Flint::GraphicsEngine("Sandbox", 1, Validation, Flint::Core::BackendAPI::Vulkan);
	auto device = engine.createDevice();
	auto window = engine.createWindow(device.get(), "Sandbox");

	{
		auto camera = Flint::MonoCamera(glm::vec3(0.0f), static_cast<float>(1280) / 720);
		camera.m_MovementBias = 50;

		Flint::FrameTimer timer;

		const auto windowHandle = window.get();
		while (!g_EventSystem.shouldClose())
		{
			auto update = engine.updateWindow(windowHandle);
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

			spdlog::info("Frame rate: {}", Flint::FrameTimer::FramesPerSecond(duration), " ns");

			update.wait();
			camera.update();
		}

		const auto ss = timer.tick();
	}

	return 0;
}