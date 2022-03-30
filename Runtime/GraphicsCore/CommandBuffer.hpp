// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"
#include "RenderTarget.hpp"
#include "WireFrame.hpp"

namespace Flint
{
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
	enum class DrawCallMode : uint8_t
	{
		Vertex,
		Indexed
	};

	/**
	 * Flint command buffer object.
	 * Command buffers are used to record commands and to be submitted to the device to execute those commands.
	 * These command buffers are allocated by the command buffer allocator, which manages the command buffer life time.
	 */
	template<class DerivedT, class CommandBufferAllocatorT, class ScreenBoundRenderTargetT, class OffScreenRenderTargetT, class GraphicsPipelineT, class ComputePipelineT, class BufferT, class GeometryStoreT, class ResourcePackageT, class ImageT, class SwapChainT, class QueryT>
	class CommandBuffer : public FObject
	{
		friend CommandBufferAllocatorT;

	protected:
		/**
		 * Protected constructor.
		 * This constructor can only be called by the inherited class, or the command buffer allocator.
		 *
		 * @param pAllocator The command buffer allocator which allocated this object.
		 */
		explicit CommandBuffer(CommandBufferAllocatorT* pAllocator) : pAllocator(pAllocator) {}

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
		virtual void BeginBufferRecording(const ScreenBoundRenderTargetT* pRenderTarget) = 0;

		/**
		 * Begin secondary command buffer recording.
		 * Before binding objects to a command buffer, it must first start recording.
		 *
		 * @param pRenderTarget The render target to which the commands are bound to.
		 */
		virtual void BeginBufferRecording(const OffScreenRenderTargetT* pRenderTarget) = 0;

		/**
		 * Bind a render target to the command buffer.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void BindRenderTarget(const ScreenBoundRenderTargetT* pRenderTarget) = 0;

		/**
		 * Bind a render target to the current command buffer as secondary.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void BindRenderTargetSecondary(const ScreenBoundRenderTargetT* pRenderTarget) = 0;

		/**
		 * Bind a render target to the command buffer.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void BindRenderTarget(const OffScreenRenderTargetT* pRenderTarget) = 0;

		/**
		 * Bind a render target to the current command buffer as secondary.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void BindRenderTargetSecondary(const OffScreenRenderTargetT* pRenderTarget) = 0;

		/**
		 * Clear a render target data to its default.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void ClearRenderTarget(const ScreenBoundRenderTargetT* pRenderTarget) = 0;

		/**
		 * Clear a render target data to its default.
		 *
		 * @param pRenderTarget The render target pointer.
		 */
		virtual void ClearRenderTarget(const OffScreenRenderTargetT* pRenderTarget) = 0;

		/**
		 * Bind a graphics pipeline to the command buffer.
		 *
		 * @param pGraphicsPipeline The graphics pipeline pointer.
		 */
		virtual void BindGraphicsPipeline(const GraphicsPipelineT* pGraphicsPipeline) = 0;

		/**
		 * Bind a compute pipeline to the command buffer.
		 *
		 * @param pComputePipeline The compute pipeline pointer.
		 */
		virtual void BindComputePipeline(const ComputePipelineT* pComputePipeline) = 0;

		/**
		 * Bind a vertex buffer to the command buffer.
		 *
		 * @param pBuffer The buffer pointer.
		 * @param firstBinding The first binding of the buffer. Default is 0.
		 * @param offset The offset of the buffer. Default is 0.
		 */
		virtual void BindVertexBuffer(const BufferT* pBuffer, const uint64_t firstBinding = 0, const uint64_t offset = 0) = 0;

		/**
		 * Bind a index buffer to the command buffer.
		 *
		 * @param pBuffer The buffer pointer.
		 * @param indexSize The size of a single index. Acceptable sizes are 1, 2 and 4.
		 * @param offset The offset of the buffer. Default is 0.
		 */
		virtual void BindIndexBuffer(const BufferT* pBuffer, const uint64_t indexSize, const uint64_t offset = 0) = 0;

		/**
		 * Bind a geometry store to the command buffer.
		 *
		 * @param pGeometryStore The geometry store pointer.
		 */
		virtual void BindGeometryStore(const GeometryStoreT* pGeometryStore) = 0;

		/**
		 * Bind a draw resources to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pResourcePackage The resource package to bind.
		 */
		virtual void BindResourcePackage(const GraphicsPipelineT* pPipeline, ResourcePackageT* pResourcePackage) = 0;

		/**
		 * Bind draw resources to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pResourcePackages The resource packages to bind.
		 */
		virtual void BindResourcePackages(const GraphicsPipelineT* pPipeline, const std::vector<ResourcePackageT*>& pResourcePackages) = 0;

		/**
		 * Bind an instance resource to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pResourcePackage The resource package to bind.
		 */
		virtual void BindResourcePackage(const ComputePipelineT* pPipeline, ResourcePackageT* pResourcePackage) = 0;

		/**
		 * Bind draw resources to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pResourcePackages The resource packages to bind.
		 */
		virtual void BindResourcePackages(const ComputePipelineT* pPipeline, const std::vector<ResourcePackageT*>& pResourcePackages) = 0;

		/**
		 * Bind dynamic states to the command buffer.
		 *
		 * @param pPipeline The pipeline to which the dynamic states are bound to.
		 * @param pDynamicStates The dynamic states to bind.
		 */
		virtual void BindDynamicStates(const GraphicsPipelineT* pPipeline, const DynamicStateContainer* pDynamicStates) = 0;

		/**
		 * Bind a view port to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pViewPort The view port pointer.
		 */
		virtual void BindViewPort(const GraphicsPipelineT* pPipeline, const ViewPort* pViewPort) = 0;

		/**
		 * Bind a scissor to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pScissor The scissor pointer.
		 */
		virtual void BindScissor(const GraphicsPipelineT* pPipeline, const Scissor* pScissor) = 0;

		/**
		 * Bind a line width to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pLineWidth The line width pointer.
		 */
		virtual void BindLineWidth(const GraphicsPipelineT* pPipeline, const LineWidth* pLineWidth) = 0;

		/**
		 * Bind a depth bias to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pDepthBias The depth bias pointer.
		 */
		virtual void BindDepthBias(const GraphicsPipelineT* pPipeline, const DepthBias* pDepthBias) = 0;

		/**
		 * Bind blend constants to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pBlendConstants The blend constants pointer.
		 */
		virtual void BindBlendConstants(const GraphicsPipelineT* pPipeline, const BlendConstants* pBlendConstants) = 0;

		/**
		 * Bind depth bounds to the command buffer.
		 *
		 * @param pPipeline The pipeline pointer.
		 * @param pDepthBounds The depth bounds pointer.
		 */
		virtual void BindDepthBounds(const GraphicsPipelineT* pPipeline, const DepthBounds* pDepthBounds) = 0;

		/**
		 * Bind constant data to the command buffer.
		 *
		 * @param pConstantData The constant data pointer.
		 * @param type The shader type to bind the data to.
		 */
		virtual void BindConstantData(const GraphicsPipelineT* pPipeline, const ConstantData* pConstantData, const ShaderType type) = 0;

		/**
		 * Bind dynamic states to the command buffer.
		 *
		 * @param pPipeline The pipeline to which the dynamic states are bound to.
		 * @param pDynamicStates The dynamic states to bind.
		 */
		virtual void BindDynamicStates(const ComputePipelineT* pPipeline, const DynamicStateContainer* pDynamicStates) = 0;

		/**
		 * Bind constant data to the command buffer.
		 *
		 * @param pConstantData The constant data pointer.
		 * @param type The shader type to bind the data to.
		 */
		virtual void BindConstantData(const ComputePipelineT* pPipeline, const ConstantData* pConstantData) = 0;

		/**
		 * Issue a draw call to the command buffer.
		 *
		 * @param wireFrame The wire frame to draw.
		 * @param firstInstance The first instance of the instance ID.
		 * @param instanceCount The number instances to draw.
		 * @param mode The draw call mode. Default is Indexed.
		 */
		virtual void IssueDrawCall(WireFrame& wireFrame, const uint64_t firstInstance = 0, const uint64_t instanceCount = 1, const DrawCallMode mode = DrawCallMode::Indexed) = 0;

		/**
		 * Issue a draw call to the command buffer.
		 *
		 * @param wireFrame The wire frame to draw.
		 * @param firstInstance The first instance of the instance ID.
		 * @param instanceCount The number instances to draw.
		 * @param mode The draw call mode. Default is Indexed.
		 */
		virtual void IssueDrawCall(WireFrame&& wireFrame, const uint64_t firstInstance = 0, const uint64_t instanceCount = 1, const DrawCallMode mode = DrawCallMode::Indexed) = 0;

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
		virtual void CopyImage(const ImageT* pSourceImage, const FBox3D sourceOffset, ImageT* pDestinationImage, const FBox3D destinationOffset) = 0;

		/**
		 * Copy an image to a swap chain image.
		 *
		 * @param pSourceImage The source image pointer.
		 * @param sourceOffset The offset of the source image to copy.
		 * @param pSwapChain The swap chain pointer.
		 * @param imageIndex The image index in the swap chain to which the copy is made.
		 * @param destinationOffset The swap chain image offset to copy data to.
		 */
		virtual void CopyToSwapChainImage(const ImageT* pSourceImage, const FBox3D sourceOffset, SwapChainT* pSwapChain, const uint32_t imageIndex, const FBox3D destinationOffset) = 0;

		/**
		 * Submit a secondary command buffer to be executed.
		 *
		 * @param pCommandBuffer The command buffer pointer.
		 */
		virtual void SubmitSecondaryCommandBuffer(const std::shared_ptr<DerivedT>& pCommandBuffer) = 0;

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
		virtual void IncludeSwapChain(SwapChainT* pSwapChain) = 0;

		/**
		 * Begin a query recording.
		 *
		 * @param pQuery The query to begin recording.
		 * @param index The index of the query primitive.
		 * @param requirePrecision Toggle if we need high precision. Default is false.
		 */
		virtual void BeginQuery(const QueryT* pQuery, const uint32_t index, const bool requirePrecision = false) = 0;

		/**
		 * End a query recording.
		 *
		 * @param pQuery The query to end recording.
		 * @param index The index of the query primitive.
		 */
		virtual void EndQuery(const QueryT* pQuery, const uint32_t index) = 0;

		/**
		 * Reset a query recordings.
		 * Since a query can contain multiple query primitives, we reset all of them starting from a position to n number of primitives.
		 *
		 * @param pQuery The query pointer.
		 * @param beginIndex The begin query primitive index.
		 * @param count The number of query primitives to reset.
		 */
		virtual void ResetQuery(const QueryT* pQuery, const uint32_t beginIndex, const uint32_t count) = 0;

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
		CommandBufferAllocatorT* GetAllocator() const { return pAllocator; }

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
		CommandBufferAllocatorT* pAllocator = nullptr;

		bool bIsRecording = false;
	};
}