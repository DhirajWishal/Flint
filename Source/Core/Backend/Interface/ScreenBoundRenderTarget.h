// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Core/Backend/ShaderDigest.h"

namespace Flint
{
	namespace Interface
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////
		////	Screen Bound 2D Render Target
		///////////////////////////////////////////////////////////////////////////////////////////////////

		/**
		 * Create a new screen bound 2D render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RenderTargetHandleSB2D, CreateRenderTargetSB2D, const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount);

		/**
		 * Create a screen bound 2D graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(GraphicsPipelineHandle, CreateGraphicsPipelineSB2D, const RenderTargetHandleSB2D& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec);


		/**
		 * Create a screen bound 2D compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(ComputePipelineHandle, CreateComputePipelineSB2D, const RenderTargetHandleSB2D& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Create a screen bound 2D ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RayTracingPipelineHandle, CreateRayTracingPipelineSB2D, const RenderTargetHandleSB2D& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created screen bound 2D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRenderTargetSB2D, const RenderTargetHandleSB2D& handle);

		///////////////////////////////////////////////////////////////////////////////////////////////////
		////	Screen Bound 3D Render Target
		///////////////////////////////////////////////////////////////////////////////////////////////////

		/**
		 * Create a new screen bound 3D render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RenderTargetHandleSB3D, CreateRenderTargetSB3D, const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount);

		/**
		 * Create a screen bound 3D graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(GraphicsPipelineHandle, CreateGraphicsPipelineSB3D, const RenderTargetHandleSB3D& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec);

		/**
		 * Create a screen bound 3D compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(ComputePipelineHandle, CreateComputePipelineSB3D, const RenderTargetHandleSB3D& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Create a screen bound 3D ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RayTracingPipelineHandle, CreateRayTracingPipelineSB3D, const RenderTargetHandleSB3D& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created screen bound 3D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRenderTargetSB3D, const RenderTargetHandleSB3D& handle);

		///////////////////////////////////////////////////////////////////////////////////////////////////
		////	Screen Bound Ray Traced Render Target
		///////////////////////////////////////////////////////////////////////////////////////////////////

		/**
		 * Create a new screen bound RT render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RenderTargetHandleSBRT, CreateRenderTargetSBRT, const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount);

		/**
		 * Create a screen bound RT graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(GraphicsPipelineHandle, CreateGraphicsPipelineSBRT, const RenderTargetHandleSBRT& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec);

		/**
		 * Create a screen bound RT compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(ComputePipelineHandle, CreateComputePipelineSBRT, const RenderTargetHandleSBRT& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Create a screen bound RT ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RayTracingPipelineHandle, CreateRayTracingPipelineSBRT, const RenderTargetHandleSBRT& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created screen bound RT render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRenderTargetSBRT, const RenderTargetHandleSBRT& handle);
	}
}