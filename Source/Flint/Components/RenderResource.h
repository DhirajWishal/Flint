// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBackend\VulkanExporter.h"

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

		UniformBufferContainer mUniformBuffers;
		ScreenBoundGraphicsPipelineResource mPipelineResource = {};

		std::vector<UI64> mDrawDataIDs;
	};
}