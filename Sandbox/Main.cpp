// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanWindow.hpp"
#include "VulkanBackend/VulkanRasterizer.hpp"
#include "Core/EventSystem/EventSystem.hpp"
#include "Core/Loader/GeometryLoader.hpp"

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

			pRasterizer->registerGeometry(Flint::LoadGeometry(pEngine->getDefaultGeometryStore(), FLINT_GLTF_ASSET_PATH "Sponza/glTF/Sponza.gltf", Flint::VertexData::Position | Flint::VertexData::Normal | Flint::VertexData::Texture0),
				[](const Flint::Mesh& mesh, [[maybe_unused]] const Flint::Geometry& geometry)
				{
					Flint::MeshRasterizer rasterizer;

					static auto vertexShader = Flint::ShaderCode("Shaders/Debugging/vert.spv", Flint::ShaderType::Vertex);
					static auto fragmentShader = Flint::ShaderCode("Shaders/Debugging/frag.spv", Flint::ShaderType::Fragment);

					// Setup graphics specification.
					auto& specification = rasterizer.getSpecification();
					specification.m_CacheFile = "Debugging.bin";
					specification.m_VertexShader = vertexShader;
					specification.m_FragmentShader = fragmentShader;

					auto& binding = specification.m_InputBindings.emplace_back(Flint::InputBindingType::VertexData);
					binding.add(0, Flint::VertexAttributeType::Vec3_32);	// layout (location = 0) in vec3 inPosition;
					binding.add(1, Flint::VertexAttributeType::Vec3_32);	// layout (location = 1) in vec3 inNormal;
					binding.add(2, Flint::VertexAttributeType::Vec2_32);	// layout (location = 2) in vec2 inTextureCoordinates;

					return rasterizer;
				}
			);

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