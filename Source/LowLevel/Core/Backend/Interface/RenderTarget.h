// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Core/Maths/Vector/Vector2.h"
#include "Core/Backend/ShaderDigest.h"

/**
 * The render target is the object which holds information about where the objects are rendered to. There are two
 * main types,
 * 1. Screen Bound (which renderes to a screen).
 * 2. Off Screen (which isnt rendered to a screen).
 *
 * Each of these 2 types have 3 sub-types,
 * 1. 2D.
 * 2. 3D.
 * 3. Ray Traced (hardware accelerated if supported or else the compute shader will be used).
 *
 * Render targets also manage its pipelines. These pipelines are of 3 types,
 * 1. Graphics (which uses rasterization).
 * 2. Compute.
 * 3. Ray Tracing.
 *
 * All the objects are assigned to a render target prior to execution/ render.
 */

namespace Flint
{
	namespace Interface
	{
		/**
		 * Render Target Extent structure.
		 */
		struct RenderTargetExtent {
			RenderTargetExtent() = default;
			RenderTargetExtent(UI32 width, UI32 height) : mWidth(width), mHeight(height) {}
			RenderTargetExtent(const Vector2& vector)
				: mWidth(static_cast<UI32>(vector.x)), mHeight(static_cast<UI32>(vector.y)) {}

			UI32 mWidth = 0;
			UI32 mHeight = 0;
		};

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
		inline RenderTargetHandleSB2D(*CreateRenderTargetSB2D)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Create a screen bound 2D graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		inline GraphicsPipelineHandle(*CreateGraphicsPipelineSB2D)(const RenderTargetHandleSB2D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created graphics pipeline.
		 *
		 * @param handle: The graphics pipeline handle.
		 */
		inline void (*DestroyGraphicsPipelineSB2D)(const GraphicsPipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound 2D compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		inline ComputePipelineHandle(*CreateComputePipelineSB2D)(const RenderTargetHandleSB2D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created compute pipeline.
		 *
		 * @param handle: The compute pipeline handle.
		 */
		inline void (*DestroyComputePipelineSB2D)(const ComputePipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound 2D ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		inline RayTracingPipelineHandle(*CreateRayTracingPipelineSB2D)(const RenderTargetHandleSB2D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created ray tracing pipeline.
		 *
		 * @param handle: The ray tracing pipeline handle.
		 */
		inline void (*DestroyRayTracingPipelineSB2D)(const RayTracingPipelineHandle& handle) = nullptr;

		/**
		 * Destroy a created screen bound 2D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetSB2D)(const RenderTargetHandleSB2D& handle) = nullptr;

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
		inline RenderTargetHandleSB3D(*CreateRenderTargetSB3D)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Create a screen bound 3D graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		inline GraphicsPipelineHandle(*CreateGraphicsPipelineSB3D)(const RenderTargetHandleSB3D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created graphics pipeline.
		 *
		 * @param handle: The graphics pipeline handle.
		 */
		inline void (*DestroyGraphicsPipelineSB3D)(const GraphicsPipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound 3D compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		inline ComputePipelineHandle(*CreateComputePipelineSB3D)(const RenderTargetHandleSB3D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created compute pipeline.
		 *
		 * @param handle: The compute pipeline handle.
		 */
		inline void (*DestroyComputePipelineSB3D)(const ComputePipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound 3D ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		inline RayTracingPipelineHandle(*CreateRayTracingPipelineSB3D)(const RenderTargetHandleSB3D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created ray tracing pipeline.
		 *
		 * @param handle: The ray tracing pipeline handle.
		 */
		inline void (*DestroyRayTracingPipelineSB3D)(const RayTracingPipelineHandle& handle) = nullptr;

		/**
		 * Destroy a created screen bound 3D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetSB3D)(const RenderTargetHandleSB3D& handle) = nullptr;

		///////////////////////////////////////////////////////////////////////////////////////////////////
		////	Screen Bound Ray Traced Render Target
		///////////////////////////////////////////////////////////////////////////////////////////////////

		/**
		 * Create a new ray traced render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		inline RenderTargetHandleSBRT(*CreateRenderTargetSBRT)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Create a screen bound ray traced graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		inline GraphicsPipelineHandle(*CreateGraphicsPipelineSBRT)(const RenderTargetHandleSBRT& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created graphics pipeline.
		 *
		 * @param handle: The graphics pipeline handle.
		 */
		inline void (*DestroyGraphicsPipelineSBRT)(const GraphicsPipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound ray traced compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		inline ComputePipelineHandle(*CreateComputePipelineSBRT)(const RenderTargetHandleSBRT& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created compute pipeline.
		 *
		 * @param handle: The compute pipeline handle.
		 */
		inline void (*DestroyComputePipelineSBRT)(const ComputePipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound ray traced ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray traced pipeline handle.
		 */
		inline RayTracingPipelineHandle(*CreateRayTracingPipelineSBRT)(const RenderTargetHandleSBRT& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created ray traced pipeline.
		 *
		 * @param handle: The ray traced pipeline handle.
		 */
		inline void (*DestroyRayTracingPipelineSBRT)(const RayTracingPipelineHandle& handle) = nullptr;

		/**
		 * Destroy a created ray traced render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetSBRT)(const RenderTargetHandleSBRT& handle) = nullptr;

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
		inline RenderTargetHandleOS2D(*CreateRenderTargetOS2D)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Create a screen bound 2D graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		inline GraphicsPipelineHandle(*CreateGraphicsPipelineOS2D)(const RenderTargetHandleOS2D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created graphics pipeline.
		 *
		 * @param handle: The graphics pipeline handle.
		 */
		inline void (*DestroyGraphicsPipelineOS2D)(const GraphicsPipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound 2D compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		inline ComputePipelineHandle(*CreateComputePipelineOS2D)(const RenderTargetHandleOS2D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created compute pipeline.
		 *
		 * @param handle: The compute pipeline handle.
		 */
		inline void (*DestroyComputePipelineOS2D)(const ComputePipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound 2D ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		inline RayTracingPipelineHandle(*CreateRayTracingPipelineOS2D)(const RenderTargetHandleOS2D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created ray tracing pipeline.
		 *
		 * @param handle: The ray tracing pipeline handle.
		 */
		inline void (*DestroyRayTracingPipelineOS2D)(const RayTracingPipelineHandle& handle) = nullptr;

		/**
		 * Destroy a created off screen 2D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetOS2D)(const RenderTargetHandleOS2D& handle) = nullptr;

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
		inline RenderTargetHandleOS3D(*CreateRenderTargetOS3D)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Create a screen bound 3D graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		inline GraphicsPipelineHandle(*CreateGraphicsPipelineOS3D)(const RenderTargetHandleOS3D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created graphics pipeline.
		 *
		 * @param handle: The graphics pipeline handle.
		 */
		inline void (*DestroyGraphicsPipelineOS3D)(const GraphicsPipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound 3D compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		inline ComputePipelineHandle(*CreateComputePipelineOS3D)(const RenderTargetHandleOS3D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created compute pipeline.
		 *
		 * @param handle: The compute pipeline handle.
		 */
		inline void (*DestroyComputePipelineOS3D)(const ComputePipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound 3D ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		inline RayTracingPipelineHandle(*CreateRayTracingPipelineOS3D)(const RenderTargetHandleOS3D& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created ray tracing pipeline.
		 *
		 * @param handle: The ray tracing pipeline handle.
		 */
		inline void (*DestroyRayTracingPipelineOS3D)(const RayTracingPipelineHandle& handle) = nullptr;

		/**
		 * Destroy a created off screen 3D render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetOS3D)(const RenderTargetHandleOS3D& handle) = nullptr;

		///////////////////////////////////////////////////////////////////////////////////////////////////
		////	Off Screen Ray Traced Render Target
		///////////////////////////////////////////////////////////////////////////////////////////////////

		/**
		 * Create a new ray traced render target.
		 *
		 * @param deviceHandle: The device handle to which the render target is created to.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of frame buffers to include. If passed 0, the buffer count will be the device
		 * recommended ammount. If the buffer count is set to std::numeric_limits<UI32>::max(), the maximum
		 * supported buffer count is set.
		 * @return The render target handle.
		 */
		inline RenderTargetHandleOSRT(*CreateRenderTargetOSRT)(const DeviceHandle& deviceHandle, const RenderTargetExtent& extent, UI32 bufferCount) = nullptr;

		/**
		 * Create a screen bound ray traced graphics pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The graphics pipeline handle.
		 */
		inline GraphicsPipelineHandle(*CreateGraphicsPipelineOSRT)(const RenderTargetHandleOSRT& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created graphics pipeline.
		 *
		 * @param handle: The graphics pipeline handle.
		 */
		inline void (*DestroyGraphicsPipelineOSRT)(const GraphicsPipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound ray traced compute pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The compute pipeline handle.
		 */
		inline ComputePipelineHandle(*CreateComputePipelineOSRT)(const RenderTargetHandleOSRT& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created compute pipeline.
		 *
		 * @param handle: The compute pipeline handle.
		 */
		inline void (*DestroyComputePipelineOSRT)(const ComputePipelineHandle& handle) = nullptr;

		/**
		 * Create a screen bound ray traced ray tracing pipeline.
		 *
		 * @param renderTargetHandle: The render target handle to which the pipeline is bound to.
		 * @param digests: The shader digests to use.
		 * @return The ray tracing pipeline handle.
		 */
		inline RayTracingPipelineHandle(*CreateRayTracingPipelineOSRT)(const RenderTargetHandleOSRT& renderTargetHandle, const std::vector<ShaderDigest>& digests) = nullptr;

		/**
		 * Destroy a created ray tracing pipeline.
		 *
		 * @param handle: The ray tracing pipeline handle.
		 */
		inline void (*DestroyRayTracingPipelineOSRT)(const RayTracingPipelineHandle& handle) = nullptr;

		/**
		 * Destroy a created ray traced render target.
		 *
		 * @param handle: The render target handle to be destroyed.
		 */
		inline void (*DestroyRenderTargetOSRT)(const RenderTargetHandleOSRT& handle) = nullptr;
	}
}