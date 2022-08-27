// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Scene.hpp"

/**
 * Firefly scene.
 */
class FireflyScene final : public Flint::Scene
{
public:

};

namespace Flint
{
	struct Instantiation
	{
		void bind() {}
	};

	struct FragmentShader
	{
		Instantiation instantiate() {}
	};

	namespace PostFX
	{
		class Processing {};
		class FXAA final : public Processing {};
		class SSR final : public Processing {};
	}

	namespace Materials
	{
		class Material {};
		class PBR final : public Material {};
	}

	class Renderer {};

	class Window
	{
	public:
		void show(Renderer* renderer, std::vector<PostFX::Processing*> processes) {}
	};

	void something()
	{
		Renderer renderer;
		PostFX::SSR ssrFilter;
		PostFX::FXAA fxaaFilter;

		Window window;
		window.show(&renderer, { &ssrFilter, &fxaaFilter });
	}
}