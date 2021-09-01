// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"
#include "RenderTarget.hpp"
#include "WireFrame.hpp"

namespace Flint
{
	class ScreenBoundRenderTarget;
	class OffScreenRenderTarget;
	class GraphicsPipeline;
	class ComputePipeline;
	class GeometryStore;
	class ResourceMap;

	class CommandBufferAllocator;

	struct DynamicStateContainer;

	/**
	 * Draw call mode enum.
	 * Vertex drawing will draw individual vertexes and would cost more.
	 * Indexed drawing will use the index buffer to draw and is generally more efficient.
	 */
	enum class DrawCallMode : UI8 {
		Vertex,
		Indexed
	};

	/**
	 * Flint command buffer object.
	 * Command buffers are used to record commands and to be submitted to the device to execute those commands.
	 * These command buffers are allocated by the command buffer allocator, which manages the command buffer life time.
	 */
	class CommandBuffer : public FObject
	{
		friend CommandBufferAllocator;

	protected:
		/**
		 * Protected constructor.
		 * This constructor can only be called by the inherited class, or the command buffer allocator.
		 * 
		 * @param pAllocator: The command buffer allocator which allocated this object.
		 */
		CommandBuffer(const std::shared_ptr<CommandBufferAllocator>& pAllocator) : pAllocator(pAllocator) {}

	public:
		/**
		 * Begin command buffer recording.
		 * Before binding objects to a command buffer, it must first start recording.
		 */
		virtual void BeginBufferRecording() = 0;

		/**
		 * Begin secondary command buffer recording.
		 * Before binding objects to a command buffer, it must first start recording.
		 *
		 * @param pRenderTarget: The render target to which the commands are bound to.
		 */
		virtual void BeginBufferRecording(const std::shared_ptr<ScreenBoundRenderTarget> pRenderTarget) = 0;

		/**
		 * Begin secondary command buffer recording.
		 * Before binding objects to a command buffer, it must first start recording.
		 *
		 * @param pRenderTarget: The render target to which the commands are bound to.
		 */
		virtual void BeginBufferRecording(const std::shared_ptr<OffScreenRenderTarget> pRenderTarget) = 0;

		/**
		 * Bind a render target to the command buffer.
		 *
		 * @param pRenderTarget: The render target pointer.
		 */
		virtual void BindRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) = 0;

		/**
		 * Bind a render target to the current command buffer as secondary.
		 *
		 * @param pRenderTarget: The render target pointer.
		 */
		virtual void BindRenderTargetSecondary(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) = 0;

		/**
		 * Bind a render target to the command buffer.
		 *
		 * @param pRenderTarget: The render target pointer.
		 */
		virtual void BindRenderTarget(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget) = 0;

		/**
		 * Bind a render target to the current command buffer as secondary.
		 *
		 * @param pRenderTarget: The render target pointer.
		 */
		virtual void BindRenderTargetSecondary(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget) = 0;

		/**
		 * Bind a graphics pipeline to the command buffer.
		 *
		 * @param pGraphicsPipeline: The graphics pipeline pointer.
		 */
		virtual void BindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline>& pGraphicsPipeline) = 0;

		/**
		 * Bind a compute pipeline to the command buffer.
		 *
		 * @param pComputePipeline: The compute pipeline pointer.
		 */
		virtual void BindComputePipeline(const std::shared_ptr<ComputePipeline>& pComputePipeline) = 0;

		/**
		 * Bind a geometry store to the command buffer.
		 * 
		 * @param pGeometryStore: The geometry store pointer.
		 */
		virtual void BindGeometryStore(const std::shared_ptr<GeometryStore>& pGeometryStore) = 0;

		/**
		 * Bind draw resources to the command buffer.
		 *
		 * @param pPipeline: The pipeline pointer.
		 * @param pResourceMap: The resource map to bind.
		 */
		virtual void BindResourceMap(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::shared_ptr<ResourceMap>& pResourceMap) = 0;

		/**
		 * Bind instance resources to the command buffer.
		 *
		 * @param pPipeline: The pipeline pointer.
		 * @param pResourceMap: The resource map to bind.
		 */
		virtual void BindResourceMap(const std::shared_ptr<ComputePipeline>& pPipeline, const std::shared_ptr<ResourceMap>& pResourceMap) = 0;

		/**
		 * Bind dynamic states to the command buffer.
		 *
		 * @param pPipeline: The pipeline to which the dynamic states are bound to.
		 * @param pDynamicStates: The dynamic states to bind.
		 */
		virtual void BindDynamicStates(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::shared_ptr<DynamicStateContainer>& pDynamicStates) = 0;

		/**
		 * Bind dynamic states to the command buffer.
		 *
		 * @param pPipeline: The pipeline to which the dynamic states are bound to.
		 * @param pDynamicStates: The dynamic states to bind.
		 */
		virtual void BindDynamicStates(const std::shared_ptr<ComputePipeline>& pPipeline, const std::shared_ptr<DynamicStateContainer>& pDynamicStates) = 0;

		/**
		 * Issue a draw call to the command buffer.
		 * 
		 * @param wireFrame: The wire frame to draw.
		 * @param mode: The draw call mode. Default is Indexed.
		 */
		virtual void IssueDrawCall(const WireFrame& wireFrame, const DrawCallMode mode = DrawCallMode::Indexed) = 0;

		/**
		 * Issue the compute call to the command buffer.
		 *
		 * @param groups: The compute groups.
		 */
		virtual void IssueComputeCall(const FBox3D& groups) = 0;

		/**
		 * Submit a secondary command buffer to be executed.
		 * 
		 * @param pCommandBuffer: The command buffer pointer.
		 */
		virtual void SubmitSecondaryCommandBuffer(const std::shared_ptr<CommandBuffer>& pCommandBuffer) = 0;

		/**
		 * Execute all the secondary commands.
		 * After executing the submitted secondary command buffers, the submitted command buffers are removed from this buffer.
		 */
		virtual void ExecuteSecondaryCommands() = 0;

		/**
		 * Unbind the render target from the current buffer.
		 */
		virtual void UnbindRenderTarget() = 0;

		/**
		 * End the currently active command buffer recording.
		 */
		virtual void EndBufferRecording() = 0;

	public:
		/**
		 * Get the allocator of this command buffer.
		 * 
		 * @param The allocator pointer.
		 */
		const std::shared_ptr<CommandBufferAllocator> GetAllocator() const { return pAllocator; }

		/**
		 * Check if the command buffer is valid.
		 * If the allocator is terminated, all of its command buffers are marked as invalid.
		 * 
		 * @return Boolean stating if the command buffer is valid.
		 */
		const bool IsValid() const { return !bIsTerminated; }

		/**
		 * Check if the buffer is in the recording state.
		 * 
		 * @return The boolean value stating if the buffer is in the recording state or not.
		 */
		const bool IsRecording() const { return bIsRecording; }

	protected:
		/**
		 * Terminate the command buffer.
		 */
		virtual void Terminate() = 0;

	protected:
		std::shared_ptr<CommandBufferAllocator> pAllocator = nullptr;

		bool bIsRecording = false;
	};
}