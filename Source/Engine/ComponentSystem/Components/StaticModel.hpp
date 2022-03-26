// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ComponentBase.hpp"
#include "GraphicsCore/WireFrame.hpp"
#include "GraphicsCore/GeometryStore.hpp"

namespace Flint
{
	namespace Components
	{
		/**
		 * Static model structure.
		 */
		struct StaticModel 
		{
			std::vector<GraphicsCore::WireFrame> mWireFrames = {};
			std::shared_ptr<Graphics::GeometryStore> pGeometryStore = nullptr;
		};
	}
}