// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"
#include "RenderTarget.hpp"
#include "WireFrame.hpp"

namespace Flint
{
	class SwapChain;
	class ScreenBoundRenderTarget;
	class OffScreenRenderTarget;
	class GraphicsPipeline;
	class ComputePipeline;
	class Buffer;
	class GeometryStore;
	class ResourcePackage;
	class Query;

	class CommandBufferAllocator;

	struct DynamicStateContainer;
	struct ViewPort;
	struct Scissor;
	struct LineWidth;
	struct DepthBias;
	struct BlendConstants;
	struct DepthBounds;
	struct ConstantData;

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
		 * @param pAllocator The command buffer allocator which allocated this object.
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
		 * @param pRenderTarget The render target to which the commands are bound to.
		 */
		virtual void BeginBufferRecording(const ScreenBoundRenderTarget* pRenderTarget) = 0;

		/**
		 * Begin secondary command buffer recording.
		 * Before binding objects to a command buffer, it must first start recording.
		 *
		 * @param pRenderTarget The render target to which the commands are bound to.
		 */
		virtual void BeginBufferRecording(const OffScreenRenderTarget* pRenderTarget) = 0;

		/**
		 * Bind a render target to the command buffer.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void BindRenderTarget(const ScreenBoundRenderTarget* pRenderTarget) = 0;

		/**
		 * Bind a render target to the current command buffer as secondary.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void BindRenderTargetSecondary(const ScreenBoundRenderTarget* pRenderTarget) = 0;

		/**
		 * Bind a render target to the command buffer.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void BindRenderTarget(const OffScreenRenderTarget* pRenderTarget) = 0;

		/**
		 * Bind a render target to the current command buffer as secondary.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void BindRenderTargetSecondary(const OffScreenRenderTarget* pRenderTarget) = 0;

		/**
		 * Clear a render target data to its default.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void ClearRenderTarget(const ScreenBoundRenderTarget* pRenderTarget) = 0;

		/**
		 * Clear a render target data to its default.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void ClearRenderTarget(const OffScreenRenderTarget* pRenderTarget) = 0;

		/**
		 * Bind a graphics pipeline to the command buffer.
		 *
		 * @param pGraphicsPipeline The graphics pipeline pointer.
		 */
		virtual void BindGraphicsPipeline(const GraphicsPipeline* pGraphicsPipeline) = 0;

		/**
		 * Bind a compute pipeline to the command buffer.
		 *
		 * @param pComputePipeline The compute pipeline pointer.
		 */
		virtual void BindComputePipeline(const ComputePipeline* pComputePipeline) = 0;

		/**
		 * Bind a vertex buffer to the command buffer.
		 *
		 * @param pBuffer The buffer pointer.
		 * @param firstBinding The first binding of the buffer. Default is 0.
		 * @param offset The offset of the buffer. Default is 0.
		 */
		virtual void BindVertexBuffer(const Buffer* pBuffer, const UI64 firstBinding = 0, const UI64 offset = 0) = 0;

		/**
		 * Bind a index buffer to the command buffer.
		 *
		 * @param pBuffer The buffer pointer.
		 * @param indexSize The size of a single index. Acceptable sizes are 1, 2 and 4.
		 * @param offset The offset of the buffer. Default is 0.
		 */
		virtual void BindIndexBuffer(const Buffer* pBuffer, const UI64 indexSize, const UI64 offset = 0) = 0;

		/**
		 * Bind a geometry store to the command buffer.
		 *
		 * @param pGeometryStore The geometry store pointer.
		 */
		virtual void BindGeometryStore(const GeometryStore* pGeometryStore) = 0;

		/**
		 * Bind a draw resources to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pResourcePackage The resource package to bind.
		 */
		virtual void BindResourcePackage(const GraphicsPipeline* pPipeline, ResourcePackage* pResourcePackage) = 0;

		/**
		 * Bind draw resources to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pResourcePackages The resource packages to bind.
		 */
		virtual void BindResourcePackages(const GraphicsPipeline* pPipeline, const std::vector<ResourcePackage*>& pResourcePackages) = 0;

		/**
		 * Bind an instance resource to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pResourcePackage The resource package to bind.
		 */
		virtual void BindResourcePackage(const ComputePipeline* pPipeline, ResourcePackage* pResourcePackage) = 0;

		/**
		 * Bind draw resources to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pResourcePackages The resource packages to bind.
		 */
		virtual void BindResourcePackages(const ComputePipeline* pPipeline, const std::vector<ResourcePackage*>& pResourcePackages) = 0;

		/**
		 * Bind dynamic states to the command buffer.
		 *
		 * @param pPipeline The pipeline to which the dynamic states are bound to.
		 * @param pDynamicStates The dynamic states to bind.
		 */
		virtual void BindDynamicStates(const GraphicsPipeline* pPipeline, const DynamicStateContainer* pDynamicStates) = 0;

		/**
		 * Bind a view port to the command buffer.
		 * 
		 * @param pPipeline The pipeline pointer.
		 * @param pViewPort The view port pointer.
		 */
		virtual void BindViewPort(const GraphicsPipeline* pPipeline, const ViewPort* pViewPort) = 0;

		/**
		 * Bind a scissor to the command buffer.
		 * 
		 * @param pPipeline The pipeline pointer.
		 * @param pScissor The scissor pointer.
		 */
		virtual void BindScissor(const GraphicsPipeline* pPipeline, const Scissor* pScissor) = 0;

		/**
		 * Bind a line width to the command buffer.
		 * 
		 * @param pPipeline The pipeline pointer.
		 * @param pLineWidth The line width pointer.
		 */
		virtual void BindLineWidth(const GraphicsPipeline* pPipeline, const LineWidth* pLineWidth) = 0;

		/**
		 * Bind a depth bias to the command buffer.
		 * 
		 * @param pPipeline The pipeline pointer.
		 * @param pDepthBias The depth bias pointer.
		 */
		virtual void BindDepthBias(const GraphicsPipeline* pPipeline, const DepthBias* pDepthBias) = 0;

		/**
		 * Bind blend constants to the command buffer.
		 * 
		 * @param pPipeline The pipeline pointer.
		 * @param pBlendConstants The blend constants pointer.
		 */
		virtual void BindBlendConstants(const GraphicsPipeline* pPipeline, const BlendConstants* pBlendConstants) = 0;

		/**
		 * Bind depth bounds to the command buffer.
		 * 
		 * @param pPipeline The pipeline pointer.
		 * @param pDepthBounds The depth bounds pointer.
		 */
		virtual void BindDepthBounds(const GraphicsPipeline* pPipeline, const DepthBounds* pDepthBounds) = 0;

		/**
		 * Bind constant data to the command buffer.
		 * 
		 * @param pConstantData The constant data pointer.
		 * @param type The shader type to bind the data to.
		 */
		virtual void BindConstantData(const GraphicsPipeline* pPipeline, const ConstantData* pConstantData, const ShaderType type) = 0;

		/**
		 * Bind dynamic states to the command buffer.
		 *
		 * @param pPipeline The pipeline to which the dynamic states are bound to.
		 * @param pDynamicStates The dynamic states to bind.
		 */
		virtual void BindDynamicStates(const ComputePipeline* pPipeline, const DynamicStateContainer* pDynamicStates) = 0;

		/**
		 * Bind constant data to the command buffer.
		 *
		 * @param pConstantData The constant data pointer.
		 * @param type The shader type to bind the data to.
		 */
		virtual void BindConstantData(const ComputePipeline* pPipeline, const ConstantData* pConstantData) = 0;

		/**
		 * Issue a draw call to the command buffer.
		 *
		 * @param wireFrame The wire frame to draw.
		 * @param firstInstance The first instance of the instance ID.
		 * @param instanceCount The number instances to draw.
		 * @param mode The draw call mode. Default is Indexed.
		 */
		virtual void IssueDrawCall(WireFrame& wireFrame, const UI64 firstInstance = 0, const UI64 instanceCount = 1, const DrawCallMode mode = DrawCallMode::Indexed) = 0;

		/**
		 * Issue a draw call to the command buffer.
		 *
		 * @param wireFrame The wire frame to draw.
		 * @param firstInstance The first instance of the instance ID.
		 * @param instanceCount The number instances to draw.
		 * @param mode The draw call mode. Default is Indexed.
		 */
		virtual void IssueDrawCall(WireFrame&& wireFrame, const UI64 firstInstance = 0, const UI64 instanceCount = 1, const DrawCallMode mode = DrawCallMode::Indexed) = 0;

		/**
		 * Issue the compute call to the command buffer.
		 *
		 * @param groups The compute groups.
		 */
		virtual void IssueComputeCall(const FBox3D& groups) = 0;

		/**
		 * Copy image data from one image to another.
		 *
		 * @param pSourceImage The source image pointer.
		 * @param sourceOffset The source offset of the image to copy from.
		 * @param pDestinationImage The destination image pointer.
		 * @param destinationOffset The destination offset of the image to copy to.
		 */
		virtual void CopyImage(const Image* pSourceImage, const FBox3D sourceOffset, Image* pDestinationImage, const FBox3D destinationOffset) = 0;

		/**
		 * Copy an image to a swap chain image.
		 *
		 * @param pSourceImage The source image pointer.
		 * @param sourceOffset The offset of the source image to copy.
		 * @param pSwapChain The swap chain pointer.
		 * @param imageIndex The image index in the swap chain to which the copy is made.
		 * @param destinationOffset The swap chain image offset to copy data to.
		 */
		virtual void CopyToSwapChainImage(const Image* pSourceImage, const FBox3D sourceOffset, SwapChain* pSwapChain, const UI32 imageIndex, const FBox3D destinationOffset) = 0;

		/**
		 * Submit a secondary command buffer to be executed.
		 *
		 * @param pCommandBuffer The command buffer pointer.
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

		/**
		 * Include a swap chain to the command buffers.
		 * Swap chain synchronization is done when the command buffer is submitted to draw. This means that there isn't another way to synchronize swap chains explicitly.
		 * This function provides a way to attach an external swap chain to the command buffer so that it too can be submitted and signaled.
		 *
		 * @param pSwapChain The swap chain to include.
		 */
		virtual void IncludeSwapChain(SwapChain* pSwapChain) = 0;

		/**
		 * Begin a query recording.
		 *
		 * @param pQuery The query to begin recording.
		 * @param index The index of the query primitive.
		 * @param requirePrecision Toggle if we need high precision. Default is false.
		 */
		virtual void BeginQuery(const Query* pQuery, const UI32 index, const bool requirePrecision = false) = 0;

		/**
		 * End a query recording.
		 *
		 * @param pQuery The query to end recording.
		 * @param index The index of the query primitive.
		 */
		virtual void EndQuery(const Query* pQuery, const UI32 index) = 0;

		/**
		 * Reset a query recordings.
		 * Since a query can contain multiple query primitives, we reset all of them starting from a position to n number of primitives.
		 *
		 * @param pQuery The query pointer.
		 * @param beginIndex The begin query primitive index.
		 * @param count The number of query primitives to reset.
		 */
		virtual void ResetQuery(const Query* pQuery, const UI32 beginIndex, const UI32 count) = 0;

		/**
		 * Add a synchronization command to the command buffer.
		 * Synchronization is needed in between two commands to make sure that the previous command is properly executed before continuing with the next command.
		 */
		virtual void Synchronize() = 0;

	public:
		/**
		 * Get the allocator of this command buffer.
		 *
		 * @return The allocator pointer.
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