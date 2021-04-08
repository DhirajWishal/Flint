// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Components/SceneComponent.h"

namespace Flint
{
	void SceneComponent::DestroyRenderResources(RenderResource& renderResource)
	{
		//mPipeline.(renderResource.mUniformBuffers);
		renderResource.mPipelineResource.Terminate();
	}
}
