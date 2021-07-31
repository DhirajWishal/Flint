// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Backend/Core/DataType.hpp"
#include <glm/glm.hpp>

struct Vertex 
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

std::pair<std::vector<Vertex>, std::vector<UI32>> LoadAsset(const std::filesystem::path& path);

struct ImageData
{
	void* pImageData = nullptr;
	Flint::FBox3D mExtent;
};

ImageData LoadImage(const std::filesystem::path& path);
void DestroyImage(ImageData data);
