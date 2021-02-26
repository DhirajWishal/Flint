// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Objects/WireFrame.h"
#include "Core/Backend/Pipeline.h"
#include "RenderResource.h"

namespace Flint
{
	/**
	 * Scene component object.
	 * This object contains information for an object/ wire frame to be rendered.
	 */
	class SceneComponent {
	public:
		SceneComponent() {}

		/**
		 * Destroy a created render resource.
		 * 
		 * @param renderResource: The render resource to be destroyed.
		 */
		void DestroyRenderResources(RenderResource& renderResource);

		/**
		 * Get the uniform buffers required by the pipeline.
		 * 
		 * @return The uniform buffer container with the respective uniform buffers.
		 */
		Backend::UniformBufferContainer GetUniformBuffers() const { return pPipeline->CreateUniformBuffers(); }

		WireFrame mWireFrame;
		Backend::Pipeline* pPipeline = nullptr;
	};
}