// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanWindow.hpp"
#include "VulkanBackend/VulkanRasterizer.hpp"
#include "Core/EventSystem/EventSystem.hpp"

#ifdef FLINT_DEBUG
constexpr auto Validation = true;

#else
constexpr auto Validation = false;

#endif

// We need to do this because of SDL.
#ifdef main
#undef main

#endif

int main()
{
	auto instance = Flint::VulkanBackend::VulkanInstance("Sandbox", 1, Validation);
	auto eventSystem = Flint::EventSystem();

	// Note that the scope is here because the engine needs to be destructed BEFORE destroying the instance.
	{
		auto pEngine = instance.createEngine();
		{
			auto pWindow = pEngine->createWindow("Sandbox");
			auto pRasterizer = pEngine->createRasterizer(1280, 720, pWindow->getFrameCount(), { Flint::Defaults::ColorAttachmentDescription, Flint::Defaults::DepthAttachmentDescription });

			while (!eventSystem.shouldClose())
			{
				pRasterizer->update();
				pWindow->update();
				const auto events = eventSystem.poll();

				if (events != Flint::EventType::None)
				{
					// handle all the events here.
				}
			}
		}
	}

	return 0;
}