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
		////	Off Screen 2D Render Target
		///////////////////////////////////////////////////////////////////////////////////////////////////

		/**
		 * Create a new off screen 2D render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RenderTargetHandleOS2D, CreateRenderTargetOS2D, const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount);

		/**
		 * Create a off screen 2D graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(GraphicsPipelineHandle, CreateGraphicsPipelineOS2D, const RenderTargetHandleOS2D& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created graphics pipeline.
		 *
		 * @param handle: The graphics pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyGraphicsPipelineOS2D, const GraphicsPipelineHandle& handle);

		/**
		 * Create a off screen 2D compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(ComputePipelineHandle, CreateComputePipelineOS2D, const RenderTargetHandleOS2D& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created compute pipeline.
		 *
		 * @param handle: The compute pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyComputePipelineOS2D, const ComputePipelineHandle& handle);

		/**
		 * Create a off screen 2D ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RayTracingPipelineHandle, CreateRayTracingPipelineOS2D, const RenderTargetHandleOS2D& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created ray tracing pipeline.
		 *
		 * @param handle: The ray tracing pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRayTracingPipelineOS2D, const RayTracingPipelineHandle& handle);

		/**
		 * Destroy a created off screen 2D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRenderTargetOS2D, const RenderTargetHandleOS2D& handle);

		///////////////////////////////////////////////////////////////////////////////////////////////////
		////	Off Screen 3D Render Target
		///////////////////////////////////////////////////////////////////////////////////////////////////

		/**
		 * Create a new off screen 3D render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RenderTargetHandleOS3D, CreateRenderTargetOS3D, const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount);

		/**
		 * Create a off screen 3D graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(GraphicsPipelineHandle, CreateGraphicsPipelineOS3D, const RenderTargetHandleOS3D& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created graphics pipeline.
		 *
		 * @param handle: The graphics pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyGraphicsPipelineOS3D, const GraphicsPipelineHandle& handle);

		/**
		 * Create a off screen 3D compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(ComputePipelineHandle, CreateComputePipelineOS3D, const RenderTargetHandleOS3D& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created compute pipeline.
		 *
		 * @param handle: The compute pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyComputePipelineOS3D, const ComputePipelineHandle& handle);

		/**
		 * Create a off screen 3D ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RayTracingPipelineHandle, CreateRayTracingPipelineOS3D, const RenderTargetHandleOS3D& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created ray tracing pipeline.
		 *
		 * @param handle: The ray tracing pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRayTracingPipelineOS3D, const RayTracingPipelineHandle& handle);

		/**
		 * Destroy a created off screen 3D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRenderTargetOS3D, const RenderTargetHandleOS3D& handle);

		///////////////////////////////////////////////////////////////////////////////////////////////////
		////	Off Screen Ray Traced Render Target
		///////////////////////////////////////////////////////////////////////////////////////////////////

		/**
		 * Create a new off screen RT render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RenderTargetHandleOSRT, CreateRenderTargetOSRT, const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount);

		/**
		 * Create a off screen RT graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(GraphicsPipelineHandle, CreateGraphicsPipelineOSRT, const RenderTargetHandleOSRT& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created graphics pipeline.
		 *
		 * @param handle: The graphics pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyGraphicsPipelineOSRT, const GraphicsPipelineHandle& handle);

		/**
		 * Create a off screen RT compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(ComputePipelineHandle, CreateComputePipelineOSRT, const RenderTargetHandleOSRT& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created compute pipeline.
		 *
		 * @param handle: The compute pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyComputePipelineOSRT, const ComputePipelineHandle& handle);

		/**
		 * Create a off screen RT ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(RayTracingPipelineHandle, CreateRayTracingPipelineOSRT, const RenderTargetHandleOSRT& renderTargetHandle, const std::vector<ShaderDigest>& digests);

		/**
		 * Destroy a created ray tracing pipeline.
		 *
		 * @param handle: The ray tracing pipeline handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRayTracingPipelineOSRT, const RayTracingPipelineHandle& handle);

		/**
		 * Destroy a created off screen RT render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRenderTargetOSRT, const RenderTargetHandleOSRT& handle);
	}
}