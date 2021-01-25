// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.h"

namespace Flint
{
	namespace Interface
	{
		/**
		 * Create a new graphics pipline.
		 * 
		 */
		inline GraphicsPipelineHandle(*CreateGraphicsPipeline)() = nullptr;
		inline void (*DestroyGraphicsPipeline)(const GraphicsPipelineHandle& handle) = nullptr;

		inline ComputePipelineHandle(*CreateComputePipeline)() = nullptr;
		inline void (*DestroyComputePipeline)(const ComputePipelineHandle& handle) = nullptr;

		inline RayTracingPipelineHandle(*CreateRayTracingPipeline)() = nullptr;
		inline void (*DestroyRayTracingPipeline)(const RayTracingPipelineHandle& handle) = nullptr;
	}
}