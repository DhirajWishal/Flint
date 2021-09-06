// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/ImGuiAdapter.hpp"

namespace Flint
{
	/**
	 * Flint ImGui scene panel.
	 * This will be used to render the scene.
	 */
	class ScenePanel
	{
	public:
		ScenePanel(ImGuiTextureContainer* pContainer);
		~ScenePanel();
	};
}