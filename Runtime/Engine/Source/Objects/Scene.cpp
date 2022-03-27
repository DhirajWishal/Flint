// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Objects/Scene.hpp"

namespace Flint
{
	void Scene::Update(const uint64_t delta)
	{
		for (auto const& [index, store] : mGameObjects)
			mGameObjectUpdateHandlers[index](*store, delta);
	}
}