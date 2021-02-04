// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Handles.h"
#include "Core/Backend/ShaderDigest.h"

namespace Flint
{
	namespace Interface
	{
		/**
		 * Create a new shader module.
		 *
		 * @param deviceHandle: The device to which the shader is bound to.
		 * @param digest: The shader digest.
		 * @return The created shader module.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(ShaderModule, CreateShaderModule, const DeviceHandle& deviceHandle, const ShaderDigest& digest);

		/**
		 * Destroy a created shader module.
		 *
		 * @param handle: The shader module handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyShaderModule, const ShaderModule& handle);

		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyGraphicsPipeline, const GraphicsPipelineHandle& handle);
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyComputePipeline, const ComputePipelineHandle& handle);
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRayTracingPipeline, const RayTracingPipelineHandle& handle);
	}
}