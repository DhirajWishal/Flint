// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.h"
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
		inline ShaderModule(*CreateShaderModule)(const DeviceHandle& deviceHandle, const ShaderDigest& digest) = nullptr;

		/**
		 * Destroy a created shader module.
		 *
		 * @param handle: The shader module.
		 */
		inline void (*DestroyShaderModule)(const ShaderModule& sModule) = nullptr;

		inline GraphicsPipelineHandle(*CreateGraphicsPipeline)() = nullptr;
		inline void (*DestroyGraphicsPipeline)(const GraphicsPipelineHandle& handle) = nullptr;

		inline ComputePipelineHandle(*CreateComputePipeline)() = nullptr;
		inline void (*DestroyComputePipeline)(const ComputePipelineHandle& handle) = nullptr;

		inline RayTracingPipelineHandle(*CreateRayTracingPipeline)() = nullptr;
		inline void (*DestroyRayTracingPipeline)(const RayTracingPipelineHandle& handle) = nullptr;
	}
}