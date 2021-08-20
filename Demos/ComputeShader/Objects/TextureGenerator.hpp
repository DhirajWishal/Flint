// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/SceneState.hpp"

class TextureGenerator
{
public:
	TextureGenerator(SceneState* pSceneState);
	~TextureGenerator();

private:
	SceneState* pSceneState = nullptr;
};