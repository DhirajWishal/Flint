// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/WireFrame.hpp"
#include "GraphicsCore.hpp"

namespace Flint
{
	namespace Components
	{
		/**
		 * Geometry component.
		 */
		struct Geometry
		{
			std::vector<GraphicsCore::WireFrame> mWireFrames = {};

			GeometryStore* pGeometryStore = nullptr;
		};
	}
}
