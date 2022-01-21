// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace Flint
{
	class Scene;

	class GameObject
	{
	public:
		GameObject(Scene* pScene) : pScene(pScene) {}

	protected:
		Scene* pScene = nullptr;
	};
}