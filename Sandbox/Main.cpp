// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanWindow.hpp"
#include "VulkanBackend/VulkanRasterizer.hpp"
#include "Core/EventSystem/EventSystem.hpp"
#include "Core/Entities/StaticEntity.hpp"

#ifdef FLINT_DEBUG
constexpr auto Validation = true;

#else
constexpr auto Validation = false;

#endif

// We need to do this because of SDL.
#ifdef main
#	undef main

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
			auto pRasterizer = pEngine->createRasterizer(pWindow->getWidth(), pWindow->getHeight(), pWindow->getFrameCount(), { Flint::Defaults::ColorAttachmentDescription, Flint::Defaults::DepthAttachmentDescription });
			pWindow->setDependency(pRasterizer.get(), 0);

			auto entity = Flint::StaticEntity(*pEngine);
			entity.loadModel(FLINT_GLTF_ASSET_PATH "Sponza/glTF/Sponza.gltf");
			//entity.loadModel("E:\\Assets\\EU43_Castanea_sativa_Chestnut\\EU43_3.FBX");

			while (!eventSystem.shouldClose())
			{
				pRasterizer->update();
				pRasterizer->render(entity.getGeometry());
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