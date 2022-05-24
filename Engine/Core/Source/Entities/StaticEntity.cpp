// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Entities/StaticEntity.hpp"
#include "Core/GeometryStore.hpp"

namespace Flint
{
	void StaticEntity::loadModel(std::filesystem::path&& path, VertexData loadData)
	{
		m_Geometry = LoadGeometry(getEngine().getDefaultGeometryStore(), std::move(path), loadData);
	}
}