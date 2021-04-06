// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint\Objects\FPipelineResource.h"

namespace Flint
{
	/**
	 * Render resource object.
	 * This contains render resource of a single render entry.
	 */
	class RenderResource {
	public:
		RenderResource() {}

		/**
		 * Destroy the render resources.
		 */
		void Destroy();

		FUniformBufferContainer mUniformBuffers;
		FPipelineResource mPipelineResource = {};

		std::vector<UI64> mDrawDataIDs;
	};
}