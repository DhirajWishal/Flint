// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Components/SceneComponent.h"

namespace Flint
{
	void SceneComponent::DestroyRenderResources(RenderResource& renderResource)
	{
		pPipeline->DestroyUniformBuffers(renderResource.mUniformBuffers);
		renderResource.pPipelineResource->Terminate();
		delete renderResource.pPipelineResource;
	}
}
