// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Objects/WireFrame.h"
#include "Core/Backend/Pipeline.h"

namespace Flint
{
	/**
	 * Scene component object.
	 * This object contains information for an object/ wire frame to be rendered.
	 */
	class SceneComponent {
	public:
		SceneComponent() {}

		WireFrame mWireFrame;
		Backend::Pipeline* pPipeline = nullptr;
	};
}