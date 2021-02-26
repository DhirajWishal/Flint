// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Pipeline.h"
#include "Core/Backend/RenderTarget.h"

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

		Backend::UniformBufferContainer mUniformBuffers;
		Backend::PipelineResource* pPipelineResource = nullptr;
		Backend::EntryReference mReference = {};
	};
}