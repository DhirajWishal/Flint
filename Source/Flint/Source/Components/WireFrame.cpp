// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Components/WireFrame.h"
#include "Core/ErrorHandler/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Flint
{
	std::vector<VertexAttribute> CreateDefaultAttributes()
	{
		std::vector<VertexAttribute> attributes(2);

		attributes[0].mType = VertexAttributeType::POSITION;
		attributes[0].mSize = sizeof(float) * 4;

		attributes[1].mType = VertexAttributeType::TEXTURE_COORDINATES_0;
		attributes[1].mSize = sizeof(float) * 2;

		return attributes;
	}
}