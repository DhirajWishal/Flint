// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/Handle.h"

namespace Flint
{
	namespace Interface
	{
		FLINT_DEFINE_HANDLE(InstanceHandle);
		FLINT_DEFINE_HANDLE(DisplayHandle);
		FLINT_DEFINE_HANDLE(DeviceHandle);

		FLINT_DEFINE_HANDLE(RenderTargetHandleSB2D);	// Screen Bound 2D
		FLINT_DEFINE_HANDLE(RenderTargetHandleSB3D);	// Screen Bound 3D
		FLINT_DEFINE_HANDLE(RenderTargetHandleSBRT);	// Screen Bound Ray Traced
		FLINT_DEFINE_HANDLE(RenderTargetHandleOS2D);	// Off Screen 2D
		FLINT_DEFINE_HANDLE(RenderTargetHandleOS3D);	// Off Screen 3D
		FLINT_DEFINE_HANDLE(RenderTargetHandleOSRT);	// Off Screen Ray Traced

		FLINT_DEFINE_HANDLE(GraphicsPipelineHandle);
		FLINT_DEFINE_HANDLE(ComputePipelineHandle);
		FLINT_DEFINE_HANDLE(RayTracingPipelineHandle);

		FLINT_DEFINE_HANDLE(ShaderModule);

		FLINT_DEFINE_HANDLE(StaggingBufferHandle);
		FLINT_DEFINE_HANDLE(VertexBufferHandle);
		FLINT_DEFINE_HANDLE(IndexBufferHandle);
		FLINT_DEFINE_HANDLE(UniformBufferHandle);
	}
}